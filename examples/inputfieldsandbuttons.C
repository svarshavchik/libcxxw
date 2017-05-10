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
#include <x/w/canvas.H>
#include <x/w/button.H>
#include <x/w/shortcut.H>

#include <x/mcguffinstash.H>

#include <string>
#include <iostream>

// This is the creator lambda, that gets passed to create_mainwindow() below,
// factored out for readability.

void create_mainwindow(const x::w::main_window &main_window,
		       const auto &close_flag)
{
	// Each display element contains an optional 'appdata', a
	// generic LibCXX object, an x::ptr<x::obj>, that the library ignores,
	// and is for application to use.
	//
	// This can be a convenient place to store all display elements that
	// get created in the main application window.
	//
	// As discussed in the library's documentation, parent display
	// elements own references to their child display element, and
	// this main_window is the topmost display elements, so it's a
	// logical place to store all display elements for the application
	// to quickly reference.
	//
	// Using some innermost display element to store references to parent
	// display elements creates a circular reference, which will interfere
	// with LibCXX's object model anyway.
	//
	// We'll use x::mcguffinstash to implement a generic collection of
	// display elements identified by an arbitrary string.

	auto stash=x::mcguffinstash<>::create();

	main_window->appdata=stash;

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
	auto subject_field=factory->valign(x::w::valign::middle)
		.create_input_field({""}, {30});

	// Save it in the stash.

	stash->insert("subject", subject_field);

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

	auto text_field=factory->create_input_field({"arial"_font,
				"Hello,\n\n"},
		{30, 4});

	// Save it in the stash

	stash->insert("text", text_field);

	// The third row of the main window will have all the buttons.
	//
	// First, create a nested container, which will span both columns,
	// and also use the grid layout manager.

	factory=layout->append_row();

	// Give the container some extra padding on the top.
	factory->top_padding(3.0)
		// Span it across both columns
		.colspan(2)
		.create_container
		([&]
		 (const auto &container)
		 {
			 // Get the grid layout manager for the container.

			 x::w::gridlayoutmanager layout=container->get_layoutmanager();
			 auto factory=layout->append_row();

			 // Cancel button, on the beginning of the row.

			 auto cancel=factory->create_normal_button_with_label
				 ({"Cancel"},
				  // Esc key for a shortcut
				  {'\e'});

			 // Next to it is a "Reset" button, with an
			 // underlined "R", with an "Alt"-R shortcut.
			 // Note that "R" has to be specified in lowercase.

			 auto reset=factory->create_normal_button_with_label
				 ({"underline"_decoration,
				   "R",
				   "no"_decoration,
				   "eset"}, {"Alt",'r'});

			 // Add empty space here, between the buttons.
			 // Create a canvas display element, specify 0
			 // minimum and preferred size, with the maximum
			 // size being unspecified (effectively infinite).
			 // This will allows the grid layout manager to
			 // size the canvas spacer to fill the entire width
			 // of the edit button row.
			 factory->create_canvas([]
						(const auto &ignore)
						{
						},
						{0, 0},
						{0, 0});

			 // The "Ok" button at the end of the row.

			 auto ok=factory->create_special_button_with_label
				 ({"Ok"},
				  // Enter key for a shortcut
				  {'\n'});

			 // Specify what happens when the buttons get
			 // activated.

			 cancel->on_activate([close_flag]
					     (const x::w::busy &ignore)
					     {
						     std::cout << "Cancel"
							       << std::endl;

						     close_flag->close();
					     });

			 // Note that the Reset button's callback captures
			 // the input field object by value. This is ok,
			 // the input fields are not parent display elements
			 // of the Reset button. When the main window gets
			 // destroyed, the main window object drops its
			 // references to its display elements, including
			 // the Reset button, which then drops its reference
			 // on the input field elements, which allows them
			 // to be destroyed.

			 reset->on_activate([text_field, subject_field]
					    (const x::w::busy &ignore)
					    {
						    text_field->set("");
						    subject_field->set("");
					    });

			 ok->on_activate([close_flag]
					 (const x::w::busy &ignore)
					 {
						 std::cout << "Ok"
							   << std::endl;

						 close_flag->close();
					 });
		 },
		 x::w::new_gridlayoutmanager());
}

void inputfieldsandbuttons()
{
	x::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto main_window=
		x::w::main_window::create([&]
					  (const auto &main_window)
					  {
						  create_mainwindow(main_window,
								    close_flag);
					  });

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

	// Retrieve the stash that was constructed in the creator.

	x::mcguffinstash<> stash=main_window->appdata;

	// Retrieve the input fields, and the contents of these fields:

	x::w::input_field field=stash->get("subject");

	std::cout << "Subject: " << field->get() << std::endl
		  << std::endl;

	field=stash->get("text");

	std::cout << field->get();
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
