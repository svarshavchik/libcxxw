/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "config.h"
#include "close_flag.H"

#include <x/exception.H>
#include <x/destroy_callback.H>

#include <x/w/new_layoutmanager.H>
#include <x/w/main_window.H>
#include <x/w/gridlayoutmanager.H>
#include <x/w/gridfactory.H>
#include <x/w/label.H>
#include <x/w/text_param_literals.H>
#include <x/w/font_literals.H>
#include <string>
#include <iostream>

// This is the creator lambda, that's passed to create_mainwindow(), below,
// factored out for readability.

void create_mainwindow(const x::w::main_window &main_window)
{
	x::w::rgb light_yellow{
		x::w::rgb::maximum,
		x::w::rgb::maximum,
		(x::w::rgb::component_t)(x::w::rgb::maximum * .75)},

		blue{0, 0, x::w::rgb::maximum},

		black{};

	main_window->set_background_color
		(main_window->create_solid_color_picture(light_yellow));

	x::w::gridlayoutmanager
		layout=main_window->get_layoutmanager();

	x::w::gridfactory factory=layout->append_row();

	factory->create_label({
		 blue,
		"underline"_decoration,
		"serif; point_size=24; weight=bold"_font,

	        "Lorem ipsum\n",

		"no"_decoration,
		"sans serif; point_size=12"_font,
		black,

		"dolor sit amet, consectetur adipisicing elit, "
		"sed do eiusmod tempor incididunt ut labore et dolore mana "
		"aliqua. Ut enim ad minim veniam, quis nostrud exercitation "
		"ullamco laboris nisi ut aliquip ex ea commodo consequat. "
		"Duis aute irure dolor in reprehenderit in voluptate velit "
		"esse cillum dolore eu fugiat nulla pariatur. "
		"Excepteur sint occaecat cupidatat non proident, "
		"sunt in culpa qui officia deserunt mollit anim id est "
		"laborum."
	  },
		100.0, // Initial text width is 100 millimeters


		// Optional parameter, alignment:
		x::w::halign::center);
}

void wordwrap()
{
	x::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto main_window=x::w::main_window::create(create_mainwindow);

	main_window->on_disconnect([]
				   {
					   _exit(1);
				   });

	guard(main_window->connection_mcguffin());

	main_window->set_window_title("Hello world!");

	main_window->on_delete
		([close_flag]
		 {
			 close_flag->close();
		 });

	main_window->show_all();

	close_flag->wait();
}

int main(int argc, char **argv)
{
	try {
		wordwrap();
	} catch (const x::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
