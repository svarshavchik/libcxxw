/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "config.h"
#include "close_flag.H"

#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/weakcapture.H>

#include <x/w/main_window.H>
#include <x/w/gridlayoutmanager.H>
#include <x/w/gridfactory.H>
#include <x/w/button.H>
#include <x/w/focusable.H>
#include <string>
#include <iostream>

void create_mainwindow(const x::w::main_window &main_window)
{
	x::w::gridlayoutmanager
		layout=main_window->get_layoutmanager();

	layout->col_alignment(0, x::w::halign::center);

	auto button1=layout->append_row()
		->create_normal_button_with_label("Button 1");

	auto button2=layout->append_row()
		->create_normal_button_with_label("Button 2: disable button 1");

	auto button3=layout->append_row()
		->create_normal_button_with_label("Button 3: enable button 1");

	auto button4=layout->append_row()
		->create_normal_button_with_label("Button 4: button 2 gets focus before button 1");

	auto button5=layout->append_row()
		->create_normal_button_with_label("Button 5: button 3 gets focus after button 2");

	auto button6=layout->append_row()
		->create_normal_button_with_label("Button 6: button 1 gets focus first");

	auto button7=layout->append_row()
		->create_normal_button_with_label("Button 7: move focus to button 1");

	// Note - normally a callback cannot capture reference to its parent
	// (or children) display elements, because this would create an
	// internal circular reference.
	//
	// In all of the following cases, a callback for a given button captures
	// a reference to another button, which is neither its parent or child.

	button2->on_activate([=]
			     (const x::w::busy &mcguffin)
			     {
				     button1->set_enabled(false);
			     });

	button3->on_activate([=]
			     (const x::w::busy &mcguffin)
			     {
				     button1->set_enabled(true);
			     });

	button4->on_activate([=]
			     (const x::w::busy &mcguffin)
			     {
				     button2->get_focus_before(button1);
			     });

	button5->on_activate([=]
			     (const x::w::busy &mcguffin)
			     {
				     button3->get_focus_after(button2);
			     });

	button6->on_activate([=]
			     (const x::w::busy &mcguffin)
			     {
				     button1->get_focus_first();
			     });

	button6->on_keyboard_focus([]
				   (x::w::focus_change f)
				   {
					   std::cout << "Button 6 in focus: "
						     << x::w::in_focus(f)
						     << std::endl;
				   });
	button6->on_key_event
		([]
		 (const x::w::all_key_events_t &ke,
		  const x::w::busy &mcguffin)
		 {
			 if (std::holds_alternative<const x::w::key_event *>(ke))
			 {
				 auto keptr=std::get<const x::w::key_event *>
					 (ke);

				 std::cout << (keptr->keypress
					       ? "press ":"release ");
				 if (keptr->unicode)
					 std::cout << "U+" << keptr->unicode;
				 else
					 std::cout << "keysym "
						   << keptr->keysym;
				 std::cout << std::endl;
			 }

			 if (std::holds_alternative<const std::u32string_view *>
			     (ke))
			 {
				 // Buttons don't receive pasted unicode text,
				 // this is for demo purposes.
			 }

			 return false;
		 });

	button7->on_activate([=]
			     (const x::w::busy &mcguffin)
			     {
				     button1->request_focus();
			     });
}

void focusables()
{
	x::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto main_window=x::w::main_window
		::create(create_mainwindow,
			 x::w::new_gridlayoutmanager{});


	main_window->on_disconnect([]
				   {
					   _exit(1);
				   });

	guard(main_window->connection_mcguffin());

	main_window->set_window_title("Focusable fields");

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
		focusables();
	} catch (const x::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}