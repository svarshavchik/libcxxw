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
#include <x/w/font_literals.H>
#include <x/w/input_field.H>
#include <x/w/container.H>
#include "spininput.H"

#include <string>
#include <iostream>
#include <sstream>
#include <optional>

void create_mainwindow(const x::w::main_window &main_window,
		       const close_flag_ref &close_flag,
		       const options &opts)
{
	x::w::gridlayoutmanager
		layout=main_window->get_layoutmanager();

	layout->row_alignment(0, x::w::valign::middle);

	x::w::gridfactory factory=layout->append_row();

	factory->create_label("Enter a number, 1-49:");

	x::w::input_field_config config{3};

	config.alignment=x::w::halign::right;
	config.maximum_size=1;

	// There are two ways to enable spin controls in an input field.

	if (!opts.custom->value)
	{
		// Using set_default_spin_control_factories() creates
		// spin control using a default, theme-specified appearance.

		config.set_default_spin_control_factories();
	}
	else
	{
		// Alternatively, set_spin_control_factories() takes two
		// callbacks parameters. Each callback receives a factory
		// as a parameter, and must create exactly one display
		// element that becomes one of the spin controls.
		//
		// This example creates two plain labels: a "-" and a "+".


		config.set_spin_control_factories
			([](const x::w::factory &factory) {
				factory->create_label({
						"liberation mono"_font,
							"-"

							});
			}, [](const x::w::factory &factory) {
				factory->create_label({
						"liberation mono"_font,
							"+"

							});
			});
	}

	auto field=factory->create_input_field("", config);

	// set_default_spin_factories()/set_spin_control_factories()
	// creates the spin controls. They're just there, they do nothing
	// by themselves.
	//
	// So first, we'll attach a validator for the input field, to
	// make the input field accept only numbers between 1 and 49.

	auto validated_int=field->set_string_validator
		([]
		 (ONLY IN_THREAD,
		  const std::string &value,
		  int *parsed_value,
		  x::w::text_param &error_message,
		  const x::w::callback_trigger_t &trigger)
		 -> std::optional<int>
		 {
			 if (parsed_value)
			 {
				 if (*parsed_value > 0 && *parsed_value <= 49)
					 return *parsed_value;
			 }
			 else
			 {
				 if (value.empty())
				 {
					 return std::nullopt;
				 }
			 }

			 error_message="Must enter a number 1-49";
			 return std::nullopt;
		 },
		 []
		 (int n)
		 {
			 return std::to_string(n);
		 });

	// Now that we have our validated input field, we can make use of
	// it to drive the spin buttons.
	//
	// on_spin() installs two callbacks, one for each button.
	//
	// The first callback spins the value in the input field down by 1,
	// the second one spins it up by 1.

	field->on_spin([validated_int]
		       (ONLY IN_THREAD,
			const x::w::callback_trigger_t &trigger,
			const x::w::busy &mcguffin)
		       {
			       auto value=validated_int->validated_value.get()
				       .value_or(1);

			       if (--value)
				       validated_int->set(value);
		       },
		       [validated_int]
		       (ONLY IN_THREAD,
			const x::w::callback_trigger_t &trigger,
			const x::w::busy &mcguffin)
		       {
			       auto value=validated_int->validated_value.get()
				       .value_or(0);

			       if (++value < 50)
				       validated_int->set(value);
		       });
}

void spininputs(const options &opts)
{
	x::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	x::w::validated_input_fieldptr<char> char_input;

	x::w::validated_input_fieldptr<int> int_input;

	auto main_window=x::w::main_window::create
		([&]
		 (const auto &main_window)
		 {
			 create_mainwindow(main_window, close_flag, opts);
		 });

	main_window->on_disconnect([]
				   {
					   _exit(1);
				   });

	guard(main_window->connection_mcguffin());

	main_window->set_window_title("Spin!");
	main_window->set_window_class("main",
				      "spininput@examples.w.libcxx.com");
	main_window->on_delete
		([close_flag]
		 (ONLY IN_THREAD,
		  const x::w::busy &ignore)
		 {
			 close_flag->close();
		 });

	main_window->show_all();

	close_flag->wait();
}

int main(int argc, char **argv)
{
	try {
		options opts;

		opts.parse(argc, argv);
		spininputs(opts);
	} catch (const x::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
