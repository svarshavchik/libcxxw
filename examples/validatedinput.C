/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "config.h"
#include "close_flag.H"

#include <x/exception.H>
#include <x/destroy_callback.H>

#include <x/w/main_window.H>
#include <x/w/gridlayoutmanager.H>
#include <x/w/gridfactory.H>
#include <x/w/label.H>
#include <x/w/text_param_literals.H>
#include <x/w/input_field.H>
#include <x/w/container.H>
#include <x/w/validated_input_field_contents.H>
#include <x/w/validated_input_field.H>

#include <x/weakcapture.H>

#include <string>
#include <iostream>
#include <sstream>
#include <optional>

std::tuple<x::w::validated_input_field<char>,
	   x::w::validated_input_field<int>>
create_mainwindow(const x::w::main_window &main_window,
		  const close_flag_ref &close_flag)
{
	auto layout=main_window->gridlayout();

	layout->row_alignment(0, x::w::valign::middle);

	x::w::gridfactory factory=layout->append_row();

	factory->create_label("Enter a letter, A-Z:");

	x::w::input_field_config config{3};

	config.alignment=x::w::halign::center;
	config.maximum_size=1;

	// create_validated_input_field_contents() returns two values,
	// a callback and an objects that holds a value for the input
	// field. create_validated_input_field_contents()'s return value
	// can be passed to create_input_field() directly. The following
	// example documents the return value, for clarity.
	//
	// The first parameter is a closure that gets called IN_THREAD to
	// convert the input field into a value.
	//
	// The closure must return a std::optional<T>, where T is the validated
	// value type.
	//
	// The closure's first parameter must be either a std::string or a
	// std::u32string, and this is what was entered into the input field,
	// with leading and trailing whitespace trimmed off.
	//
	// The remaining parameters: a lock on the input field, that
	// gets passed in for convenience, and the triggering event.
	//
	// A std::nullopt return value indicates that the entered text failed
	// validation. It's up to the closure to report the parsing failure,
	// in some way. The passed-in input_field lock's stop_message() method
	// is conveniently available.
	//
	// The input field owns a reference on the installed validation
	// callback, so it can't be strongly captured, because that creates
	// a circular reference. A weak reference is an option, but the
	// input field lock object offers a stop_message() for this purpose.

	std::tuple<x::w::input_field_validation_callback,
		   x::w::validated_input_field_contents<char>> res=
		x::w::create_validated_input_field_contents(
			[]
			(ONLY IN_THREAD,
			 const std::u32string &value,
			 x::w::input_lock &lock,
			 const x::w::callback_trigger_t &trigger)
			-> std::optional<char>
			{
				// This field expects one character.

				if (value.size() == 1)
				{
					auto c=*value.c_str();

					// And it must be a letter, if so, the
					// validator closure must returned the
					// validated value.

					if ((c >= 'a' && c <= 'z')
					    ||
					    (c >= 'A' && c <= 'Z'))
						return c;
				}

				// Validation failure.

				if (value.empty())
				{
					lock.stop_message("Input required");
				}
				else
				{
					lock.stop_message(
						"Letter 'A'-'Z' required"
					);
				}

				return std::nullopt;
			},

			// The second parameter is a closure which
			// receives the most recently validated value,
			// which may be std::nullopy if the most recently
			// entered did not pass validation.
			//
			// The closure can return either a std::string or a
			// std::u32string, which becomes the contents of the
			// input field.
			[]
			(const std::optional<char> &v) -> std::u32string
			{
				if (!v)
					return U"";

				char32_t c=*v;

				if (c >= 'a' && c <= 'z')
					c += 'A'-'a';

				return {&c, &c+1};
			});

	// Passing the returned value to create_input_field() returns two
	// values: the new input field, and a validated input field object
	// that provides thread-safe access to the validated value.

	std::tuple<x::w::input_field, x::w::validated_input_field<char>> res2=
		factory->create_input_field(res, config);

	auto &[field, validated_char]=res2;

	factory=layout->append_row();

	factory->create_label("Enter a number, 0-49:");

	config=x::w::input_field_config{3};

	config.alignment=x::w::halign::right;
	config.maximum_size=2;

	// create_string_validated_input_field_contents() takes a
	// slightly different approach: use std::istream's formatted extraction
	// operator, ">>" to attempt to extract the typed in value.
	//
	// The template parameter gives the type of the extracted value.
	//
	// The first parameter is the original, raw input, before the
	// formatted extraction. The next parameter is the parsed value,
	// std::nullopt indicates a failure to extract the value. This
	// includes no input, inspect the raw value to determine that.
	// The remaining parameters have the same meaning
	// as set_validator(), the second closure is the same as
	// set_validator()'s.
	//
	// create_string_validated_input_field_contents() also returns an
	// a validation callback with an x::w::validated_input_contents<T>.

	std::tuple<x::w::input_field_validation_callback,
		   x::w::validated_input_field_contents<int>> res3=
		x::w::create_string_validated_input_field_contents<int>(
			[]
			(ONLY IN_THREAD,
			 const std::string &value,
			 std::optional<int> &parsed_value,
			 x::w::input_lock &lock,
			 const x::w::callback_trigger_t &trigger)
			{
				if (parsed_value)
				{
					if (*parsed_value >= 0 &&
					    *parsed_value <= 49)
						return;

					// Even though the int was parsed,
					// we fall through and reset the
					// parsed_value indicating a parsing
					// failure.
				}
				else
				{
					if (value.empty())
					{
						lock.stop_message(
							"Input required"
						);
						return;
					}
				}

				parsed_value.reset();
				lock.stop_message("Must enter a number 0-49");
			},
			[]
			(int n)
			{
				return std::to_string(n);
			},

			// Optional parameters (also optional for
			// create_validated_input_field_contents():

			// Initial value
			0,

			// Both create_validated_input_field_contents() and
			// create_string_validated_input_field_contents()
			// take an optional third closure. This closure
			// receives the returned value from the first
			// closure. This third closure gets invoked after
			// the returned x::w::validated_input_field
			// gets updated with the parsed value.
			//
			// This provides the means of implementing a hook that
			// gets invoked with the newly-entered value "already
			// on the books". It is not, until the first closure
			// returns, so if something gets called from the
			// first closure and it looks at the
			// x::w::validated_input_field's contents, it will
			// get the previous value, and not the current value.
			//
			// This is really the only purpose of this closure,
			// but, as an added bonus, the closure gets the new
			// value of the validated input field as its parameter.
			[]
			(ONLY IN_THREAD, const std::optional<int> &v)
			{
				if (v)
					std::cout << "You entered a " << *v
						  << std::endl;
			});

	std::tuple<x::w::input_field, x::w::validated_input_field<int>> res4=
		factory->create_input_field(res3, config);

	auto &[field2, validated_int] = res4;

	factory=layout->append_row();

	auto question=factory->create_label("What is 2+2?");

	config=x::w::input_field_config{3};
	config.maximum_size=1;

	field=factory->create_input_field("", config);

	// Creating a validated input field passes an
	// input_field_validation_callback value to create_input_field().
	//
	// on_validate() is a lower-level hook that installs an
	// input_field_validation_callback, a callback that
	// gets executed to validate the contents of the input field.
	//
	// Validation callbacks, like any other callbacks attach to the
	// display element, cannot capture a reference to the display element,
	// or any element in its parent or child hierarchy.
	//
	// create_validated_input_field_contents() and
	// create_string_validated_input_field_contents() take care of
	// constructing the callback that uses the passed-in closures.
	//
	// Returning "true" resumes normal processing. Returning "false"
	// indicates that the input field failed validation. This is used to
	// block input focus from moving to another field, if possible, keeping
	// it in the input field that failed validation.

	field->on_validate(
		[]
		(ONLY IN_THREAD,
		 x::w::input_lock &lock,
		 const x::w::callback_trigger_t &triggering_event)
		{
			if (lock.get() == "4")
				return true;

			// Use our label to throw a stop_message
			// alert. We can capture "question" for
			// this callback because it is not a
			// direct parent or child widget of the
			// callback's widget.
			lock.stop_message("No it's not");

			return false;
		}
	);
	return {validated_char, validated_int};
}

void validatedinputfields()
{
	x::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	x::w::validated_input_fieldptr<char> char_input;

	x::w::validated_input_fieldptr<int> int_input;

	auto main_window=x::w::main_window::create
		([&]
		 (const auto &main_window)
		 {
			 std::tie(char_input, int_input
				  )= create_mainwindow(main_window, close_flag);
		 });

	main_window->on_disconnect([]
				   {
					   _exit(1);
				   });

	guard(main_window->connection_mcguffin());

	main_window->set_window_title("Enter a message");
	main_window->set_window_class("main",
				      "validatedinputfields@examples.w.libcxx.com");
	main_window->on_delete
		([close_flag]
		 (ONLY IN_THREAD,
		  const x::w::busy &ignore)
		 {
			 close_flag->close();
		 });

	main_window->show_all();

	close_flag->wait();

	// create_validated_input_field_contents() and
	// create_string_validated_input_field_contents() return
	// x::w::validated_input_field<T> objects. A reference to these
	// objects is owned by the installed callback.

	std::optional<char> final_char_value=char_input->value();
	std::optional<int> final_int_value=int_input->value();

	if (final_char_value)
		std::cout << "Final char value: " << *final_char_value
			  << std::endl;

	if (final_int_value)
		std::cout << "Final int value: " << *final_int_value
			  << std::endl;
}

int main(int argc, char **argv)
{
	try {
		validatedinputfields();
	} catch (const x::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
