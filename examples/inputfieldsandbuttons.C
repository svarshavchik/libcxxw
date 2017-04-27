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
#include <x/w/input_field.H>
#include <x/w/container.H>
#include <string>
#include <iostream>

// This is the creator lambda, that's passed to create_mainwindow(), below,
// factored out for readability.

void create_mainwindow(const x::w::main_window &main_window)
{
	x::w::gridlayoutmanager
		layout=main_window->get_layoutmanager();

	// The main window's grid will have two columns. The first column
	// in each row will have a label, the second column will have a
	// text input field.

	// The labels will be aligned against the right margin of the
	// first column, to make them flush to the input field.

	// First row in the main window contains a label and an input field.

	x::w::gridfactory factory=layout->append_row();

	// Besides the right alignment, set the vertical alignment of the
	// label to middle, it looks better.

	factory->halign(x::w::halign::right)
		.valign(x::w::valign::middle)
		.create_label({"Subject:"});

	// And just in case we have some weird theme where the input field
	// is shorter in height than the label, align it vertically also.

	// The input fields initial contents are empty, and it is size to
	// be 30 columns.
	auto field=factory->valign(x::w::valign::middle)
		.create_input_field({""}, {30});

	// Second row in the main window

	factory=layout->append_row();

	// It doesn't look like we need to vertically align either the label
	// or the column.
	factory->halign(x::w::halign::right).create_label({"Text:"});

	// That's because the second row's input field will have 30 columns
	// and four rows.
	//
	// Also, let's make the input field use a non-default, proportional
	// font, and set its initial contents to "Hello".

	field=factory->create_input_field({"arial"_font,
				"Hello,\n\n"},
		{30, 4});

	// The third row of the main window will have the buttons.
	//
	// First, create a nested container, which will span both columns,
	// and also use the grid layout manager.

	factory=layout->append_row();

	// Give the container some extra padding on the top.
	factory->top_padding(3.0).create_container
		([&]
		 (const auto &container)
		 {
			 // Get the grid layout manager for the container.

			 x::w::gridlayoutmanager layout=container->get_layoutmanager();
		 },
		 x::w::new_gridlayoutmanager());
}

void inputfieldsandbuttons()
{
	x::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto main_window=x::w::main_window::create(create_mainwindow);

	main_window->on_disconnect([]
				   {
					   _exit(1);
				   });

	guard(main_window->connection_mcguffin());

	main_window->set_window_title("Enter a message");

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
		inputfieldsandbuttons();
	} catch (const x::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
