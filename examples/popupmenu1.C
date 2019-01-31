/*
** Copyright 2018-2019 Double Precision, Inc.
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
#include <x/w/container.H>
#include <x/w/listlayoutmanager.H>

#include <string>
#include <iostream>


// This is the creator lambda, that gets passed to create_mainwindow() below,
// factored out for readability.

void create_mainwindow(const x::w::main_window &main_window)
{
	x::w::gridlayoutmanager layout=main_window->get_layoutmanager();
	x::w::gridfactory factory=layout->append_row();

	// Besides the right alignment, set the vertical alignment of the
	// label to middle, it looks better.

	auto label=factory->padding(3).create_label({
			"liberation sans; point_size=20"_font,
				"Right click on me, please"
				});

	// Create a popup menu. The menu items get created just like they
	// get created for an ordinary menu, using the list layout manager.

	x::w::container popup_menu1=label->create_popup_menu
		([]
		 (const x::w::listlayoutmanager &lm)
		 {
			 lm->append_items({
					 [](ONLY IN_THREAD,
					    const auto &info)
					 {
						 std::cout << "New"
							   << std::endl;
					 },
					 "New",

					 [](ONLY IN_THREAD,
					    const auto &info)
					 {
						 std::cout << "Open"
							   << std::endl;
					 },
					 "Open",

					 [](ONLY IN_THREAD,
					    const auto &info)
					 {
						 std::cout << "Close"
							   << std::endl;
					 },
					 "Close",
				 });
		 });

	// Install a callback for the context popup action, right button
	// click. The callback captures the menu popup, by reference, and
	// show()s it.

	label->install_contextpopup_callback
		([popup_menu1]
		 (ONLY IN_THREAD,
		  const x::w::element &my_element,
		  const x::w::callback_trigger_t &trigger,
		  const x::w::busy &mcguffin)
		 {
			 popup_menu1->show();
		 });
}

void popupmenu1()
{
	x::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto main_window=x::w::main_window::create
		([&]
		 (const auto &main_window)
		 {
			 create_mainwindow(main_window);
		 });

	main_window->on_disconnect([]
				   {
					   _exit(1);
				   });

	guard(main_window->connection_mcguffin());

	main_window->set_window_title("Click the label for a popup");
	main_window->set_window_class("main",
				      "popupmenu1@examples.w.libcxx.com");
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
		popupmenu1();
	} catch (const x::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
