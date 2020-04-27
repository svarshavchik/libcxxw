/*
** Copyright 2019-2020 Double Precision, Inc.
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
#include <x/w/canvas.H>
#include <x/w/container.H>
#include <x/w/button.H>

#include <string>
#include <iostream>
#include <sstream>
#include <optional>
#include <algorithm>

x::w::validated_input_field<std::string>
create_mainwindow(const x::w::main_window &main_window,
		  const close_flag_ref &close_flag)
{
	auto layout=main_window->gridlayout();

	layout->row_alignment(0, x::w::valign::middle);

	x::w::gridfactory factory=layout->append_row();

	factory->create_label("Phone #:");

	x::w::input_field_config config{15};

	config.maximum_size=14;
	config.autoselect=true;
	config.autodeselect=true;
	auto field=factory->create_input_field("(###) ###-####", config);

	field->on_default_filter(// Valid input is digits 0-9.
				 []
				 (char32_t c)
				 {
					 return c >= '0' && c <= '9';
				 },

				 // Immutable positions in the input field.
				 {0, 4, 5, 9},

				 // Character that indicates no input.
				 '#');

	factory=layout->append_row();

	factory->create_canvas();

	factory->create_button("Ok", x::w::default_button() )
		->on_activate([close_flag]
			      (ONLY IN_THREAD,
			       const auto &trigger,
			       const auto &mcguffin)
			      {
				      close_flag->close();
			      });

	return field->set_validator
		([]
		 (ONLY IN_THREAD,
		  const std::string &value,
		  const x::w::input_field &field,
		  const x::w::callback_trigger_t &trigger)
		 -> std::optional<std::string>
		 {
			 std::string s;

			 s.reserve(10);

			 for (auto c:value)
			 {
				 if (c >= '0' && c <= '9')
					 s.push_back(c);
			 }

			 if (s.size() > 0 && s.size() < 10)
			 {
				 field->stop_message("Invalid phone number");
				 return std::nullopt;
			 }

			 return s;
		 },
		 []
		 (const std::optional<std::string> &v) -> std::string
		 {
			 std::string s="(###) ###-####";

			 auto i=s.begin();

			 if (v && v->size() == 10)
			 {
				 for (char c:*v)
				 {
					 while (*i != '#')
						 ++i;
					 *i++ = c;
				 }
			 }
			 return s;
		 });
}

void filteredinputfield()
{
	x::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	x::w::validated_input_fieldptr<char> char_input;

	x::w::validated_input_fieldptr<int> int_input;

	x::w::validated_input_fieldptr<std::string> validated_phone_number;
	auto main_window=x::w::main_window::create
		([&]
		 (const auto &main_window)
		 {
			 validated_phone_number=
				 create_mainwindow(main_window, close_flag);

			 validated_phone_number->set("2125551212");
		 });

	main_window->on_disconnect([]
				   {
					   _exit(1);
				   });

	guard(main_window->connection_mcguffin());

	main_window->set_window_title("Prompt");
	main_window->set_window_class("main",
				      "filteredinput2@examples.w.libcxx.com");
	main_window->on_delete
		([close_flag]
		 (ONLY IN_THREAD,
		  const x::w::busy &ignore)
		 {
			 close_flag->close();
		 });

	main_window->show_all();

	close_flag->wait();

	auto phone_number=validated_phone_number->validated_value.get();

	if (phone_number)
		std::cout << "Phone number: " << *phone_number << std::endl;
}

int main(int argc, char **argv)
{
	try {
		filteredinputfield();
	} catch (const x::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
