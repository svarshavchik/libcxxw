/*
** Copyright 2019-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "config.h"
#include "close_flag.H"

#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/weakcapture.H>

#include <x/w/main_window.H>
#include <x/w/gridlayoutmanager.H>
#include <x/w/gridfactory.H>
#include <x/w/label.H>
#include <x/w/font_literals.H>
#include <x/w/input_field.H>
#include <x/w/input_field_lock.H>
#include <x/w/container.H>
#include <x/w/tooltip.H>
#include <x/w/button.H>
#include <x/w/canvas.H>
#include <x/w/text_param_literals.H>

#include <string>
#include <iostream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <fenv.h>

// Maximum number of digits before the decimal point
//
// The maximum size of the input field is NDIGITS+3, for the decimal point
// and two digits after the decimal point.

#define NDIGITS 9
#define NPREC 2

template<size_t n>
constexpr uint64_t pow10()
{
	return pow10<n-1>()*10;
}

template<>
constexpr uint64_t pow10<0>()
{
	return 1;
}

// Object shared by callbacks that implements the behavior of the input field.

// The behavior implemented by the callbacks:
//
// A filter callback filters input to accept only numeric values, but also
// keep an eye out if a "+", "-", "*", or "/" was typed. This specifies
// a mathematical operator.
//
// Entering a mathematical operator takes the entered numeric value, saves it,
// and clears the input field for the next numeric value, which is followed
// by an "=", which carries out the mathematical operation and shows the
// result.
//
// Instead of a "=", typing another mathematical operator also executes the
// first mathematical operation, but then saves the result, and restarts the
// process with clearing the input field for the next numeric value.
//
// This implements a simple four-function calculator. Nothing fancy, and no
// concept of mathematical precedence. Just basic, linear, calculations.
//
// The first mathematical operator opens a static tooltip above the input
// field, that records each mathematical value and the operator, like a
// ticker tape, for visual feedback. The static tooltip gets automatically
// closed by the "=", or if the calculation is aborted by tabbing out of the
// input field, or losing the input focus in any way.

class amount_fieldObj : virtual public x::obj {

public:

	// The weak pointer to the input field. Because the callbacks capture
	// this object by value, a weak pointer to the input field must be
	// stored here, in order to avoid a circular reference:
	//
	// 1. The input field owns references to the callbacks.
	//
	// 2. Callbacks capture a reference to this amount_field object.
	//
	// 3. Callbacks invoke methods here, that need to use the input field.
	//
	// The weak pointer avoids the circular reference.

	x::weakptr<x::w::input_fieldptr> input_field;

	amount_fieldObj(const x::w::input_field &input_field)
		: input_field{input_field}
	{
	}

	~amount_fieldObj()=default;

	// Format double value to NPREC digits of precision.

	static std::string round_value(double n)
	{
		// TODO: when g++ implements std::to_chars() for
		// doubles.

		auto s=std::to_string(n);

		auto p=s.find('.');

		if (p == s.npos)
		{
			p=s.size();
			s += ".";
		}
		return (s+std::string{NPREC, '0'}).substr(0, p+(NPREC+1));
	}

	// Take whatever was entered and converted to a double, round it,
	// and make sure it fits into the field.

	static std::optional<double> validate(double n)
	{
		n=std::round(n*pow10<NPREC>())/pow10<NPREC>();

		// Make sure this value fits.
		if (round_value(n).size() <= NDIGITS+NPREC+1)
			return n;

		return std::nullopt;
	}

	// See if the given string matches our format.

	static std::optional<double> try_validate(const std::string &s)
	{
		std::istringstream i{s};
		double v;

		i >> v;

		if (!i.fail())
		{
			i.get();
			if (i.eof())
				return validate(v);
		}

		return std::nullopt;
	}

	// The next mathematical operator is represented by a function pointer
	// to one of these:

	static double add(double a, double b)
	{
		return a+b;
	}

	static double sub(double a, double b)
	{
		return a-b;
	}
	static double mul(double a, double b)
	{
		return a*b;
	}

	static double div(double a, double b)
	{
		return a/b;
	}

	// This input field's on_filter() callback.

	// Validates changes to the input field: either numeric entry, or
	// typing in one of the special characters representing an
	// operation triggers this behavior.

	void filter(ONLY IN_THREAD,
		    const x::w::input_field_filter_info &info)
	{
		// First step, is to recover a strong ref to the field.
		auto field=input_field.getptr();

		if (!field)
			return;

		// Cursor movement, we don't need to do anything

		if (info.type == x::w::input_filter_type::move_only)
			return;

		// Check for special cases we recognize: -, +, *, or /
		// typed when:
		//
		// 1) The cursor is at the end of the field.
		//
		// 2) The field's contents are non-empty
		//
		// Have to take pains, like this, to ensure that
		// negative numbers can be entered with "-" normally.

		if (info.new_contents.size() == 1 && info.size > 0 &&
		    info.starting_pos == info.size)
		{
			// Before one of these operators, we better already
			// have a valid numeric value, here.

			auto v=try_validate(x::w::input_lock{field}.get());

			if (v)
			{
				double (*op)(double, double)=nullptr;

				// This is what we recognize.
				switch (info.new_contents[0]) {
				case '-':
					op=&sub;
					break;
				case '+':
					op=&add;
					break;
				case '*':
					op=&mul;
					break;
				case '/':
					op=&div;
					break;
				case '=':
					final_op(IN_THREAD, field, info, *v);
					return;
				}

				if (op)
				{
					char descr[2]=
						{
						 (char)info.new_contents[0],
						 '\0'
						};

					next_op(IN_THREAD, field,
						info, op, *v, descr);
					return;
				}
			}
		}

		// Not a special operator. Validate numeric input.

		// First pass. If new text is getting inserted, it
		// must consists of all digits and at most one
		// decimal point, and consist of digits 0-9, and
		// an optional leading dash.

		bool first_pos=true;
		size_t dots=0;

		for (auto c:info.new_contents)
		{
			if (c == '-' && first_pos)
				;
			else if (c == '.')
				++dots;
			else if (c < '0' || c > '9')
				return;
			first_pos=false;
		}

		if (dots > 1)
			return;

		// Second pass, a more thorough check.

		// Compute, in advance, the new contents of the
		// input field, what update() would do.

		auto contents=x::w::input_lock{field}.get();

		contents=contents.substr(0, info.starting_pos)

			// new_contents is a std::u32string, but we
			// checked it, above.
			+ std::string
			{
			 info.new_contents.begin(),
			 info.new_contents.end()
			} + contents.substr(info.starting_pos+info.n_delete);

		// If the new contents are empty, or if they pass validation,
		// the proposed change is acceptable.
		if (contents.empty() || contents == "-" ||
		    try_validate(contents))
			info.update();
	}

	// The input field's on_keyboard_focus callback.

	// Determine if the input field lost keyboard focus, then close
	// the static tooltip, if it exists (and is, presumably, open).

	void keyboard_focus(ONLY IN_THREAD,
			    x::w::focus_change change)
	{
		// First step, is to recover a strong ref to the field.
		auto field=input_field.getptr();

		if (!field)
			return;

		if (!x::w::in_focus(change))
			close_tooltip(IN_THREAD, field);
	}

private:

	// Static tooltip, and the latest goings on there.

	struct static_tooltip_info {
		x::w::container tooltip; //< The static tooltip itself.

		// The accumulated value of the operation.
		double accumulator;

		// The most recently entered operator.
		double (*op_func)(double, double);
	};

	// The currently shown static tooltip.
	std::optional<static_tooltip_info> opened_tooltip;

	// Execute the next mathematical operation.

	// This is invoked in two circumstances. In either case, the
	// tooltip is open with a saved accumulated value, and the next
	// operation, and the next mathematical operand value gets passed
	// in. The saved operator func, op_func() gets invoked with the
	// saved accumulated value, and the next mathematical operand value.
	//
	// The next operand value was entered into the input field, followed
	// by an operator or '='. In the first case, the result of the
	// saved operation becomes the new accumulated value, and the next
	// operator gets saved as the new op_func. In the second case,
	// the result of the saved operation becomes the final value in the
	// input field, and the static tooltip popup gets closed.
	//
	// new_value gets set to the result of the mathematical operation,
	// and execute_next_op() returns the rounded result of the
	// mathematical operation, or a nullopt if a mathematical exception
	// occurred.
	//
	// In this manner, the four-function calculation gets carried out
	// using full double precision, but the input field ends up showing
	// the rounded result.

	std::optional<double> execute_next_op(double next_operand,
					      double &new_value)
	{
		fenv_t orig_fenv;

		// We need to intelligently detect mathematical exceptions,
		// and reject the operations.

		if (feholdexcept(&orig_fenv) || !opened_tooltip)
			return std::nullopt;

		new_value=(*opened_tooltip->op_func)
			(opened_tooltip->accumulator, next_operand);

		auto math_except=fetestexcept(FE_ALL_EXCEPT & ~FE_INEXACT);
		feclearexcept(FE_ALL_EXCEPT);
		fesetenv(&orig_fenv);

		if (math_except)
			return std::nullopt;

		return validate(new_value);
	}

	// A mathematical operation was requested.

	// If this is the first one, we open the static tooltip, and save
	// the initial value and operation there; and subsequent operations
	// get appended there.

	void next_op(ONLY IN_THREAD,
		     const x::w::input_field &field,
		     const x::w::input_field_filter_info &info,
		     double (*op_func)(double, double),
		     double value,
		     const char *op_descr)
	{
		// Prepare the line to add to the opened tooltip.

		std::ostringstream o;

		o << value << " " << op_descr;

		if (!opened_tooltip)
		{
			// First operation, create the static tooltip.

			x::w::static_tooltip_config config;

			// Specify that the static tooltip gets positioned
			// above the input field.
			config.affinity=x::w::attached_to::above_or_below;

			auto tooltip=field->create_static_tooltip
				(IN_THREAD,
				 [&]
				 (const x::w::container &c)
				 {
					 x::w::gridlayoutmanager glm=
						 c->get_layoutmanager();
					 auto f=glm->append_row();

					 f->halign(x::w::halign::right);

					 f->create_label({
							  "mono"_theme_font,
							  o.str(),
						 });
				 },
				 // Optional parameter, an (not really)
				 // aggregate parameter than specifies the
				 // tooltip's layout manager (defaults to
				 // the grid layout manager, and the tooltip's
				 // settings. Either one or the other
				 // values may be left out of the fake
				 // aggregate parameter.
				 {
					 x::w::new_gridlayoutmanager{},
					 config
				 });

			tooltip->show_all();

			// Save the initial value and the first operator.
			opened_tooltip={tooltip, value, op_func};
		}
		else
		{
			// Tooltip already open, another operator.

			// First, execute the previous operator.

			double new_value;

			auto ret=execute_next_op(value, new_value);

			if (!ret)
				return;

			value=*ret;

			// Then, append another line to the tooltip popup.

			x::w::gridlayoutmanager glm=
				opened_tooltip->tooltip->get_layoutmanager();

			auto f=glm->append_row();
			f->halign(x::w::halign::right);

			f->create_label({
					 "mono"_theme_font,
					 o.str(),
				})->show();

			// But don't let the tooltip grow indefinitely.
			//
			// Once it exceeds five rows, remove the first row,
			// and replace the now-first row with ellipsis.
			// The next time we end up here this ends up removing
			// the ellipsis, and replacing the next line, now the
			// new first line with ellipsis.

			if (glm->rows() > 5)
			{
				glm->remove_row(0);

				f=glm->replace_cell(0, 0);
				f->halign(x::w::halign::right);
				f->create_label("...")->show();
			}

			// Save the new accumulated value and the next operator
			opened_tooltip->accumulator=new_value;
			opened_tooltip->op_func=op_func;

		}

		// Put the running total/accumulated value into the input field,
		// and select its entirety.
		show_next_value(info, value);
		info.select_all();
	}

	// After executing the next operation, show the running total, rounded
	// in the input field.
	void show_next_value(const x::w::input_field_filter_info &info,
			     double value)
	{
		auto value_str=round_value(value);

		// update() takes a std::u32string. We're plain ASCII.

		info.update(0, info.size, std::u32string{value_str.begin(),
								 value_str.end()
								 });
	}

	// '=' was typed in.

	// If the tooltip is open, execute the last entered operation.

	void final_op(ONLY IN_THREAD,
		      const x::w::input_field &field,
		      const x::w::input_field_filter_info &info,
		      double value)
	{
		if (!opened_tooltip)
			return;

		double new_value;

		auto ret=execute_next_op(value, new_value);

		if (!ret)
			return;

		show_next_value(info, *ret);
		close_tooltip(IN_THREAD, field);
	}

	// Close the tooltip

	void close_tooltip(ONLY IN_THREAD, const x::w::input_field &field)
	{
		if (opened_tooltip)
		{
			// Removing the tooltip consists of calling
			// the input field's remove_tooltip(). We also
			// clear the std::optional that holds the tooltip
			// info, which includes our own reference to the
			// static tooltip container popup.

			field->remove_tooltip(IN_THREAD);
			opened_tooltip.reset();
		}
	}
};

typedef x::ref<amount_fieldObj> amount_field;

x::w::validated_input_field<double>
create_mainwindow(const x::w::main_window &main_window,
		  const close_flag_ref &close_signal)
{
	x::w::gridlayoutmanager
		layout=main_window->get_layoutmanager();

	layout->row_alignment(0, x::w::valign::middle);

	x::w::gridfactory factory=layout->append_row();

	factory->create_label("Amount:");

	x::w::input_field_config config{NDIGITS+NDIGITS+NPREC+1+1};
	// 1 more for cursor.

	config.alignment=x::w::halign::right;
	config.maximum_size=NDIGITS+NPREC+1;

	// In some circumstances the entire contents of the input field
	// is select_all()ed, so flip this flag to drop the selection when
	// the input field loses its input focus.

	config.autodeselect=true;

	auto field=factory->create_input_field("", config);

	// Install an input field validator, to conveniently convert what's
	// typed in into a double value.

	x::w::validated_input_field<double>
		validated_value=field->set_string_validator
		([]
		 (ONLY IN_THREAD,
		  const std::string &value,
		  double *parsed_value,
		  const x::w::input_field &f,
		  const x::w::callback_trigger_t &trigger)
		 -> std::optional<double>
		 {
			 if (parsed_value)
			 {
				 // Official input field validator borrows
				 // the validator from amount_fieldObj.
				 auto res=amount_fieldObj::validate
					 (*parsed_value);

				 if (res)
					 return res;
			 }
			 else
			 {
				 if (value.empty())
					 return 0.0;
			 }

			 f->stop_message("Invalid number");
			 return std::nullopt;
		 },
		 []
		 (double n)
		 {
			 auto s=amount_fieldObj::round_value(n);

			 // Return an empty string instead of 0.00

			 if (std::find_if(s.begin(), s.end(),
					  []
					  (char c)
					  {
						  return c != '0' && c != '.';
					  }) == s.end())
				 s="";
			 return s;
		 });

	// Create the amount_field object that manages this input_field,
	// and install all the callbacks that do the heavy lifting.

	auto this_amount_field=amount_field::create(field);

	field->on_filter
		([this_amount_field]
		 (ONLY IN_THREAD,
		  const x::w::input_field_filter_info &info)
		 {
			 this_amount_field->filter(IN_THREAD, info);
		 });

	field->on_keyboard_focus
		([this_amount_field]
		 (ONLY IN_THREAD,
		  x::w::focus_change change,
		  const x::w::callback_trigger_t &trigger)
		 {
			 this_amount_field->keyboard_focus(IN_THREAD, change);
		 });

	// "Ok" button on the next row.

	factory=layout->append_row();
	factory->create_canvas();
	auto ok=factory->create_button("Ok", {
			x::w::default_button(),
			x::w::shortcut('\n'),
		});

	ok->on_activate([close_signal]
			(ONLY IN_THREAD,
			 const x::w::callback_trigger_t &trigger,
			 const x::w::busy &ignore)
			{
				close_signal->close();
			});

	return validated_value;
}

void statictooltip()
{
	x::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	x::w::validated_input_fieldptr<double> validator;

	auto main_window=x::w::main_window::create
		([&]
		 (const auto &main_window)
		 {
			 validator=create_mainwindow(main_window, close_flag);
		 });

	main_window->on_disconnect([]
				   {
					   _exit(1);
				   });

	guard(main_window->connection_mcguffin());

	main_window->set_window_title("Calculator");
	main_window->set_window_class("main",
				      "statictooltip@examples.w.libcxx.com");
	main_window->on_delete
		([close_flag]
		 (ONLY IN_THREAD,
		  const x::w::busy &ignore)
		 {
			 close_flag->close();
		 });

	main_window->show_all();

	close_flag->wait();

	auto value=validator->validated_value.get();

	std::cout << "Final value: ";

	if (!value)
		std::cout << "(none)";
	else
		std::cout << *value;

	std::cout << std::endl;
}

int main(int argc, char **argv)
{
	try {
		statictooltip();
	} catch (const x::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
