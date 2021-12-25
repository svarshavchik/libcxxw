/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "config.h"
#include "close_flag.H"

#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/config.H>

#include <x/w/main_window.H>
#include <x/w/main_window_appearance.H>
#include <x/w/screen_positions.H>
#include <x/w/gridlayoutmanager.H>
#include <x/w/gridfactory.H>
#include <x/w/label.H>
#include <x/w/text_param_literals.H>
#include <x/w/font_literals.H>
#include <string>
#include <iostream>

// This is the creator lambda, that gets passed to create_mainwindow() below,
// factored out for readability.

void create_mainwindow(const x::w::main_window &main_window)
{
	auto layout=main_window->gridlayout();

	x::w::gridfactory factory=layout->append_row();

	x::w::label_config config;

	config.widthmm=100.0; // Initial text width is 100 millimeters

	// Optional parameter, alignment:
	config.alignment=x::w::halign::center;

	factory->create_label
		(
		 {
		  // x::w::rgb values specify the color of the following
		  // text. <x/w/rgb.h> declares several constants for standard
		  // HTML 3.2 colors, like x::w::blue:

		  x::w::blue,
		  "underline"_decoration,

		  // "name"_font - string literal
		  "liberation serif; point_size=24; weight=bold"_font,

		  "Lorem ipsum\n",

		  "no"_decoration,

		  // "name"_theme_font - font specified by the current theme/
		  //
		  // The "label" font is used for ordinary labels
		  "label; point_size=12"_theme_font,
		  x::w::black,

		  "dolor sit amet, consectetur adipisicing elit, "
		  "sed do eiusmod tempor incididunt ut labore et dolore mana "
		  "aliqua. Ut enim ad minim veniam, quis nostrud exercitation "
		  "ullamco laboris nisi ut aliquip ex ea commodo consequat. "
		  "Duis aute irure dolor in reprehenderit in voluptate velit "
		  "esse cillum dolore eu fugiat nulla pariatur. "
		  "Excepteur sint occaecat cupidatat non proident, "
		  "sunt in culpa qui officia deserunt mollit anim id est "
		  "laborum."
		 }, config);
}

void wordwrap()
{
	x::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	// Configuration filename where we save the window's position.

	std::string configfile=
		x::configdir("wordwraplabel@examples.w.libcxx.com")
		+ "/windows";

	// Load the saved window position.
	//
	// x::w::screen_positions captures the position and size of
	// main_windows. If the configuration file exists, the previously
	// captured positions and sizes of main_windows get loaded. Nothing
	// happens if the file does not exist.
	auto pos=x::w::screen_positions::create(configfile);

	// It's possible to capture more than one main_window's position and
	// size, and save it. Each main_window must have a unique label, that
	// identifies the stored position.
	//
	// Creating a new main_window with an optional screen_position and
	// a label ends up reopening the window (eventually, when it gets
	// show()n) in its former position and size (hopefully).
	//
	// This has no effect if no memorized position was loaded from the
	// the configuration (which includes the situation where the
	// configuration file does not exist).
	//
	// Passing an optional x::w::main_window_config as the first parameter
	// to main_window's constructor specifies optional main window settings.

	x::w::main_window_config config{"main"};

	// set_name_and_position() sets the main window's unique label, and
	// restores it from the specified x::w::screen_positions, if one was
	// saved
	//
	// NOTE: the screen_positions parameter that gets passed in here must
	// remain in scope and not get destroyed until the main window's
	// constructor returns.

	config.restore(pos);

	// Obtain main window's appearance

	x::w::const_main_window_appearance appearance=config.appearance;

	x::w::rgb light_yellow
		{
		 x::w::rgb::maximum,
		 x::w::rgb::maximum,
		 (x::w::rgb_component_t)(x::w::rgb::maximum * .75)
		};

	// const_main_window_appearance is a cached constant object. It
	// remains referenced by the new main window, and the connection
	// thread can access it at any time.
	//
	// Thread safety is ensured by contract, because it is a constant
	// object. Modifying an appearance object involves invoking it's
	// modify() and passing it a closure, or a callable object.
	//
	// modify() makes a new copy of the appearance object and invokes
	// the closure with the new, temporarily-modifiable object
	// as its parameter. The closure can then modify its members.
	//
	// modify() returns a new modified appearance object, a
	// const_main_window_appearance once again, that, now safely constant
	// again, can replace the original config.appearance.

	config.appearance=appearance->modify
		([&]
		 (const x::w::main_window_appearance &custom_appearance)
		 {
			 custom_appearance->background_color=light_yellow;
		 });

	auto main_window=
		x::w::main_window::create(config, create_mainwindow);

	main_window->on_disconnect([]
				   {
					   _exit(1);
				   });

	guard(main_window->connection_mcguffin());

	main_window->set_window_title("Hello world!");
	main_window->set_window_class("main", "wordwraplabel@examples.w.libcxx.com");
	main_window->on_delete
		([close_flag]
		 (THREAD_CALLBACK,
		  const x::w::busy &ignore)
		 {
			 close_flag->close();
		 });

	main_window->show_all();

	close_flag->wait();

	// Before terminating the most recent window position and save
	// needs to be saved into a screen_positions object:

	main_window->save(pos);

	// In this case we're using the initial screen_positions that were
	// loaded from the configuration file. Specifying an existing label
	// replaces the existing saved position with the same label.
	//
	// It's also possible to default-construct a new screen_positions,
	// and use it.
	//
	// Multiple main_windows must have a unique label, each.
	//
	// Finally, the screen_positions gets save()d into the configuration
	// file, for next time:

	pos->save(configfile);
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
