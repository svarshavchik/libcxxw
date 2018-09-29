/*
** Copyright 2017 Double Precision, Inc.
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
#include <x/w/font_literals.H>
#include <x/w/image.H>
#include <string>
#include <iostream>

void testlabel()
{
	// Set up a destructor guard. The library automatically creates a
	// background thread to talk to the X server. When the main_window
	// object goes out of scope and gets destroyed, the connection thread
	// will stop, but not immediately. Some cleanup tasks must be done.
	//
	// This is not strictly necessary, but let's do a good job of
	// cleaning up after ourselves, to make sure that testlabel() does
	// not return until all display-related resources have been released
	// and the display server's connection thread stops.
	//
	// The guard object must be declared before the main_window object.

	x::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	// The main application window.

	auto main_window=x::w::main_window
		::create(// Creator lambda, for the new main window
			 []
			 (const x::w::main_window &main_window)
			 {
				 // The creator lambda for the main application
				 // window.
				 //
				 // Since we specified a gridlayoutmanager
				 // for this window (see below), this is what
				 // get_layoutmanager() will give us, here.
				 x::w::gridlayoutmanager
				     layout=main_window->get_layoutmanager();

				 // Center the display elements in column #0,
				 // the first (and the only) column in the grid
				 // that's about to get created.

				 layout->col_alignment(0, x::w::halign::center);

				 // The grid is currently empty. Append a
				 // new row to the grid, and return a factory
				 // that creates new elements in the new
				 // grid row.
				 x::w::gridfactory factory=
				     layout->append_row();

				 // Pass an optional label_config parameter
				 // to create_label().

				 x::w::label_config hello_world_label_config;

				 // Center the label's rows. This is
				 // an optional parameter, and defaults
				 // to x::w::halign::left. "right"
				 // is also an option (other halign
				 // values are not used by the label
				 // display element).

				 hello_world_label_config.alignment=
					 x::w::halign::center;

				 // Pad the next new grid element with
				 // an 8 millimeter border, and create an
				 // x::w::label element.
				 factory->padding(8.0).create_label({

						 // First label row
					"Hello world!\n",

						// Second label row, switch
						// to a non-default font.
					"liberation mono;point_size=40"_font,
					"Here I come!"
				     }, hello_world_label_config);

				 // Create a factory for the second row.
				 factory=layout->append_row();

				 // Also pad the new element
				 factory->padding(8);
				 // Create an image element.
				 factory->create_image("dandelion-flower.jpg");
			 },

			 // Our main window will use the grid layout manager...

			 // ... but we already knew that. However, for
			 // completeness, this is the default value for the
			 // second parameter to create(), that specifies the
			 // grid layout manager for the new main window.
			 //
			 // This is why when the creator lambda gets invoked
			 // above, it expects to see a gridlayoutmanager.
			 // This parameter, if specified, must be a subclass
			 // of x::w::new_layoutmanager, and this is one such
			 // subclass.

			 x::w::new_gridlayoutmanager{});

	// If the connection to the display server goes away, we don't have
	// many options...
	//
	// For this simple program, close_flag->close(), like the more
	// orderly on_delete() does, will also work.
	//
	// _exit() is for emergencies. Can't use exit() because the library
	// will try to wait for its internal execution thread to finish, but
	// it'll still be running (but doing nothing of importance).

	main_window->on_disconnect([]
				   {
					   _exit(1);
				   });

	// Retrieve the mcguffin for the underlying display connection, and
	// guard it. We won't return from this function until the underlying
	// display connection completely goes away.

	guard(main_window->connection_mcguffin());

	// Set our title.
	main_window->set_window_title("Hello world!");

	// Set the window manager class instance and resource identifiers.
	// This sets the internal WM_CLASS window property that some window
	// managers use for window-specific settings.

	main_window->set_window_class("main",
				      "helloworld@examples.w.libcxx.com");

	// Install a callback lambda that the connection thread invokes when
	// it gets a "close window" message from the display server.
	main_window->on_delete
		([close_flag]
		 (ONLY IN_THREAD,
		  const x::w::busy &ignore)
		 {
			 close_flag->close();
		 });

	// Show the main window, and all of its display elements.
	main_window->show_all();

	// Wait until the close window message is received.
	close_flag->wait();
}

int main(int argc, char **argv)
{
	try {
		testlabel();
	} catch (const x::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
