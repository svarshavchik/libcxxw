/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "config.h"
#include <x/mpobj.H>
#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/ref.H>
#include <x/obj.H>
#include <x/w/main_window.H>
#include <x/w/gridlayoutmanager.H>
#include <x/w/gridfactory.H>
#include <x/w/date_input_field.H>
#include <x/w/label.H>
#include <x/w/canvas.H>

#include <string>
#include <iostream>
#include <sstream>

#include "close_flag.H"

#include <optional>
#include <tuple>
#include <iostream>

#include <x/ymd.H>
#include <x/locale.H>
#include <x/mpobj.H>

// A helper object that holds the current values of the starting and the
// ending date fields.
//
// The values get updated by callbacks attached to those fields. The
// starting_date and ending_date values are mutex-protected objects, using
// x::mpobj from the base LibCXX library, because they could potentially
// be accessed by multiple execution threads, the main one and the
// internal library execution thread that executes the callbacks.

struct currently_shown_datesObj : virtual public x::obj {

	x::mpobj<std::optional<x::ymd>> starting_date;
	x::mpobj<std::optional<x::ymd>> ending_date;

	// The label that prompts for the next possible action.
	const x::w::label status_label;

	currently_shown_datesObj(const x::w::label &status_label)
		: status_label{status_label}
	{
	}

	// update_label() gets invoked from the callbacks, and updates the
	// message at the bottom of the window according to the values entered
	// into the two date input fields.

	void update_label()
	{
		auto s=starting_date.get();
		auto e=ending_date.get();

		// If the starting date field is empty:
		if (!s)
		{
			status_label->update("A valid starting date needs "
					     "to be entered");
			return;
		}

		// If the ending date field is empty:
		if (!e)
		{
			status_label->update("A valid ending date needs "
					     "to be entered");
			return;
		}

		// Both valid dates are entered.

		std::string diff=std::to_string(*e - *s);

		status_label->update("Number of days between " +
				     s->format_date("%x") + " and " +
				     e->format_date("%x") + ": " +
				     diff);
	}
};

typedef x::ref<currently_shown_datesObj> currently_shown_dates;

// Create the contents of the main window. Return the two date input fields.

static std::tuple<x::w::date_input_field, x::w::date_input_field
		  > create_main_window(const x::w::main_window &mw)
{
	x::w::gridlayoutmanager lm=mw->get_layoutmanager();

	auto f=lm->append_row();

	f->halign(x::w::halign::right).create_label("Starting date:");
	x::w::date_input_field starting_date=f->create_date_input_field();

	// set() sets the value of the date field.

	// Set the initial values to the current date.

	starting_date->set( x::ymd{} );

	f=lm->append_row();
	f->halign(x::w::halign::right).create_label("Ending date:");
	x::w::date_input_field ending_date=f->create_date_input_field();

	// The label field at the bottom will display various messages of
	// varying lengths. Normally the window automatically sizes itself
	// go be just big enough to accomodate its contents. In order to
	// avoid having the window constantly adjusting its size every time
	// the message changes, add an invisible canvas spacer, 150 millimeters
	// wide. This seems to be wide enough to accomodate all messages.

	f=lm->append_row();

	f->colspan(2).create_canvas([]
				    (const auto &ignore)
				    {
				    },
				    {150, 150, 150},
				    {0, 0, 0});

	f=lm->append_row();

	// Create the aforementioned label element, a placeholder, and
	// call update_label() to set its initial contents.

	auto label=f->colspan(2).halign(x::w::halign::center)
		.padding(4).create_label("");

	auto current_values=currently_shown_dates::create(label);

	// The on_change() callback on a date input field gets executed
	// when the contents of the date input field change.

	// The on_change() callbacks gets invoked as soon as its install,
	// with the date input field's current contents. We'll use this
	// initial invocation to update the message at the bottom of the
	// window.
	//
	// The on_change() callback receives the current date entered into
	// the input field. No value gets returned if the field is empty
	// or the date could not be parsed.
	//
	// Note that both callbacks capture current_values, which has a
	// reference on the label display elements. The starting_date and
	// ending_date display elements are neither the parent nor the child
	// elements of the label display element. This does not create a
	// circular reference between the objects.

	starting_date->on_change
		([current_values]
		 (ONLY IN_THREAD,
		  const std::optional<x::ymd> &new_value,
		  const x::w::callback_trigger_t &trigger)
		 {
			 current_values->starting_date=new_value;
			 current_values->update_label();
		 });

	// However for the ending date's callback, we'll suppress taking the
	// action for the x::w::initial invocation. It serves no useful
	// purpose, the field is empty.

	ending_date->on_change
		([current_values]
		 (ONLY IN_THREAD,
		  const std::optional<x::ymd> &new_value,
		  const x::w::callback_trigger_t &trigger)
		 {
			 if (std::holds_alternative<x::w::initial>(trigger))
				 return;

			 current_values->ending_date=new_value;
			 current_values->update_label();
		 });

	return {starting_date, ending_date};
}

void dateinputfields()
{
	x::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	x::w::date_input_fieldptr starting_date, ending_date;

	auto main_window=x::w::main_window
		::create([&]
			 (const auto &mw)
			 {
				 std::tie(starting_date,
					  ending_date)=create_main_window(mw);
			 });

	main_window->set_window_title("Date calculator");
	main_window->set_window_class("main", "dateinputfield@examples.w.libcxx.com");

	guard(main_window->connection_mcguffin());

	main_window->on_disconnect([]
				   {
					   exit(1);
				   });

	main_window->on_delete
		([close_flag]
		 (ONLY IN_THREAD,
		  const auto &ignore)
		 {
			 close_flag->close();
		 });

	main_window->show_all();

	close_flag->wait();

	// get() returns the current values of the date input field.

	auto final_starting_date=starting_date->get();
	auto final_ending_date=ending_date->get();

	if (final_starting_date)
		std::cout << "Final starting date: "
			  << *final_starting_date
			  << std::endl;

	if (final_ending_date)
		std::cout << "Final ending date: "
			  << *final_ending_date
			  << std::endl;

}

int main(int argc, char **argv)
{
	try {
		// Before we do everything, set the library's locale to the
		// system environment locale.

		x::locale::base::environment()->global();

		dateinputfields();
	} catch (const x::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
