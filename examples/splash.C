/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "config.h"

#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/appid.H>

#include <x/w/main_window.H>
#include <x/w/main_window_appearance.H>
#include <x/w/gridlayoutmanager.H>
#include <x/w/gridfactory.H>
#include <x/w/label.H>
#include <x/w/text_param_literals.H>
#include "splash.H"

#include <x/pidinfo.H>
#include <x/forkexec.H>
#include <x/fditer.H>

#include <string>

std::string x::appid() noexcept
{
	return "splash.examples.w.libcxx.com";
}

// Creator for the splash window, factored out for readability.

void create_splash(const x::w::main_window &main_window)
{
	auto glm=main_window->gridlayout();

	x::w::gridfactory f=glm->append_row();

	// Not much of a splash window. Just a large, black "Loading..."
	// label.

	f->create_label({
			 "serif; point_size=48"_theme_font,
			 x::w::black,
			 "Loading..."
		});
}

x::w::main_window create_mainwindow(const options &options)
{
	// main_window's create() takes an optional initial parameter that
	// precedes the creator lambda, which is x::w::main_window_config
	// by default.
	//
	// Passing in an x::w::transparent_splash_window_config or a
	// x::w::splash_window_config creates a splash window.

	x::w::transparent_splash_window_config transparent_splash_config;

	// transparent_splash_window_config creates a splash window with a
	// transparent background color, ostensibly for properly displaying
	// a rounded border around the contents of the splash window.
	//
	// splash_window_config creates a normal non-transparent window.
	//
	// transparent_splash_window_config inherits from splash_window_config,
	// and if the display screen does not have an alpha channel
	// (for transparency), it ends up creating a normal, non-transparent
	// window by using its splash_window_config superclass.

	x::w::splash_window_config &splash_config=transparent_splash_config;

	// We specify a custom appearance of the splash window, with
	// a custom background color, a vertical gradient
	//
	// This background color also gets used for the transparent window as
	// well. This sets the background color for the internal container
	// that's just inside the transparent splash window's borders. The
	// splash window's background color is transparent, hence it appears
	// to have rounded borders. The application should not use
	// set_background_color() with a transparent main_window, but rather
	// specify the apparent background color of the splash window in the
	// splash_config.

	splash_config.appearance=splash_config.appearance->modify
		([]
		 (const x::w::main_window_appearance &custom_appearance)
		 {
			 custom_appearance->background_color=
				 x::w::linear_gradient{0, 0, 0, 1,
						       0, 0,
						       {
							{0, x::w::silver},
							{1, x::w::white},
							{2, x::w::silver}
						       }};
		 });

	// splash_window_config's border member specifies an ordinary, non-
	// rounded border.
	//
	// This is an extra field in the splash_window_config that's not a
	// part of its appearance object.

	x::w::border_infomm square_border;

	square_border.color1=x::w::black;
	square_border.width=.5;
	square_border.height=.5;

	splash_config.border=square_border;

	// Its transparent_splash_window_config also has a "border", for the
	// transparent splash window, if one ends up getting created, so
	// we configure the same border there, except that it's rounded.

	x::w::border_infomm rounded_border=square_border;

	rounded_border.rounded=true;
	rounded_border.hradius=1;
	rounded_border.vradius=1;

	transparent_splash_config.border=rounded_border;

	// main_window_config's (inherited) "name" gives the main window's
	// name (defaults to "main"). This example program starts another
	// program with a main window. If this example program opened a
	// a 2nd main window, in addition to the splash one, they must use
	// different names, so this is not strictly needed here:
	transparent_splash_config.name="splash";

	// After passing either an x::w::splash_window_config or a
	// x::w::transparent_splash_window_config to main_window's create(),
	// the next parameter is the creator lambda, as usual.

	if (options.square->value)
		return x::w::main_window::create(splash_config, create_splash);

	return x::w::main_window
		::create(transparent_splash_config, create_splash);
}

x::fd spawn_application()
{
	auto me=x::exename(); // My path

	// Compute the path to the compiled "table2" program in the same
	// directory.
	me=me.substr(0, me.rfind('/')+1) + "table2";

	x::forkexec fe{me};

	// Attach a pipe to file descriptor 3.
	//
	// table2 closes file descriptor 3 when it finishes initialization.

	x::fd pipe=fe.pipe_from(3);

	fe.spawn_detached();

	// The object for the write end of the pipe is still in the forkexec
	// object. Return from here destroys it, so only the read side of
	// the pipe is open in this process. The write side of the pipe
	return pipe;
}

void splashwindow(const options &opts)
{
	x::destroy_callback::base::guard guard;

	auto main_window=create_mainwindow(opts);

	main_window->on_disconnect([]
				   {
					   _exit(1);
				   });

	guard(main_window->connection_mcguffin());


	main_window->show_all();

	sleep(2);

	auto pipe=spawn_application();

	// Now read from the pipe until file descriptor 3 is closed.

	x::fdinputiter b{pipe}, e;

	while (b != e)
		++b;
}

int main(int argc, char **argv)
{
	try {
		options opts;

		opts.parse(argc, argv);
		splashwindow(opts);
	} catch (const x::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
