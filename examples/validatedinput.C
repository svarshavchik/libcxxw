/*
** Copyright 2018 Double Precision, Inc.
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

#include <x/weakcapture.H>

#include <string>
#include <iostream>
#include <sstream>
#include <optional>

std::tuple<x::w::validated_input_field<char>,
	   x::w::validated_input_field<int>>
create_mainwindow(const x::w::main_window &main_window,
		  const auto &close_flag)
{
	x::w::gridlayoutmanager
		layout=main_window->get_layoutmanager();

	layout->row_alignment(0, x::w::valign::middle);

	x::w::gridfactory factory=layout->append_row();

	factory->create_label("Enter a letter, A-Z:");

	x::w::input_field_config config{3};

	config.alignment=x::w::halign::center;
	config.maximum_size=1;

	auto field=factory->create_input_field("", config);

	// set_validator() installs a closure that validates the input field.
	//
	// The closure must return a std::optional<T>, where T is the validated
	// value type.
	//
	// The closure's first parameter must be either a std::string or a
	// std::u32string, and this is what was entered into the input field,
	// with leading and trailing whitespace trimmed off.
	//
	// The remaining parameters are a reference to the returned error
	// message, and the triggering event.
	//
	// set_validator returns an x::w::validated_input<T> object.

	x::w::validated_input_field<char> validated_char=field->set_validator
		([]
		 (const std::u32string &value,
		  x::w::text_param &error_message,
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
				 error_message="Input required";
			 }
			 else
			 {
				 error_message="Letter 'A'-'Z' required.";
			 }

			 return std::nullopt;
		 },

		 // The second parameter to set_validator() is a closure which
		 // receives the most recently validated value, which may be
		 // null if the most recently entered did not pass validation.
		 //
		 // The closure can return either a std::string or a
		 // std::u32string, which becomes the contents of the input
		 // field.
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

	factory=layout->append_row();

	factory->create_label("Enter a number, 0-49:");

	config=x::w::input_field_config{3};

	config.alignment=x::w::halign::right;
	config.maximum_size=2;

	field=factory->create_input_field("", config);

	// set_string_validator() takes a slightly-different approach.
	// set_string_validator uses std::istream's formatted extraction
	// operator, ">>" to attempt to extract the typed in value.
	//
	// The first closure passed to set_string_validator() also returns
	// a std::optional<T>. set_string_validator() uses >> to attempt
	// to extract the typed-in value.
	//
	// The first parameter is the original, raw input, before the
	// formatted extraction. The next parameter is a pointer to the
	// extracted value, or nullptr if the extraction failed. The extraction
	// could fail if nothing was typed, and that's what the first
	// parameter is for. The remaining parameters have the same meaning
	// as set_validator(), and the second closure is the same as
	// set_validator()'s.
	//
	// set_validator also returns an x::w::validated_input<T> object.

	auto validated_int=field->set_string_validator
		([]
		 (const std::string &value,
		  int *parsed_value,
		  x::w::text_param &error_message,
		  const x::w::callback_trigger_t &trigger)
		 -> std::optional<int>
		 {
			 if (parsed_value)
			 {
				 if (*parsed_value >= 0 && *parsed_value <= 49)
					 return *parsed_value;

				 // Even though the int was parsed, we fall
				 // through and return a null optional,
				 // indicating a parsing failure.
			 }
			 else
			 {
				 if (value.empty())
				 {
					 error_message="Input required";
					 return std::nullopt;
				 }
			 }

			 error_message="Must enter a number 0-99";
			 return std::nullopt;
		 },
		 []
		 (int n)
		 {
			 return std::to_string(n);
		 });


	factory=layout->append_row();

	factory->create_label("What is 2+2?");

	config=x::w::input_field_config{3};
	config.maximum_size=1;

	field=factory->create_input_field("", config);

	// on_validate() is an alternative that provides a raw callback that
	// gets executed to validate the contents of the input field.
	//
	// Validation callbacks, like any other callbacks attach to the
	// display element, cannot capture a reference to the display element,
	// or any element in its parent or child hierarchy.
	//
	// set_validator() and set_string_validator() take care of
	// weakly-capturing the input field reference. on_validate() leaves
	// this up to you, and merely invokes the callback, with the callback
	// responsible for fetching the reference, and looking at what's in
	// the input field.
	//
	// Returning "true" resumes normal processing. Returning "false"
	// indicates that the input field failed validation. This is used to
	// block input focus from moving to another field, if possible, keeping
	// it in the input field that failed validation.

	field->on_validate([me=x::make_weak_capture(field)]
			   (const x::w::callback_trigger_t &triggering_event)
			   {
				   auto got=me.get();

				   if (!got)
					   return true;

				   auto &[me]=*got;

				   if (x::w::input_lock{me}.get() == "4")
					   return true;

				   auto main_window=me->get_main_window();

				   if (!main_window)
					   return true;

				   main_window->error_message("No it's not.");

				   return false;
			   });
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
		 (const x::w::busy &ignore)
		 {
			 close_flag->close();
		 });

	main_window->show_all();

	close_flag->wait();

	// set_validator() and set_string_validator() returns
	// x::w::validated_input_field<T> objects. A reference to these
	// objects is owned by the installed callback.

	std::optional<char> final_char_value=char_input->validated_value.get();
	std::optional<int> final_int_value=int_input->validated_value.get();

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
