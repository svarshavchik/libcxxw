/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "config.h"
#include "close_flag.H"

#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/appid.H>

#include <x/w/main_window.H>
#include <x/w/gridlayoutmanager.H>
#include <x/w/gridfactory.H>
#include <x/w/label.H>
#include <x/w/text_param_literals.H>
#include <x/w/font_literals.H>
#include <x/w/container.H>
#include <x/w/listlayoutmanager.H>
#include <x/w/busy.H>
#include <x/threads/run.H>
#include <string>
#include <iostream>

std::string x::appid() noexcept
{
	return "popupmenu2.examples.w.libcxx.com";
}

// Temporary holding object for the most recently created context menu popup.

class most_recent_popupObj : virtual public x::obj {

public:

	x::w::containerptr created_popup;
};

typedef x::ref<most_recent_popupObj> most_recent_popup;


x::w::container create_popup_menu(const x::w::element &my_element);

// This is the creator lambda, that gets passed to create_mainwindow() below,
// factored out for readability.

void create_mainwindow(const x::w::main_window &main_window)
{
	auto layout=main_window->gridlayout();
	x::w::gridfactory factory=layout->append_row();

	auto label=factory->padding(3).create_label({
			"liberation sans; point_size=20"_font,
				"Right click on me, please"
				});

	// Install a context popup callback, but without creating a popup
	// menu. The callback captures a holding object for the popup menu,
	// and creates a new popup menu every time it gets called.
	//
	// The created menu is show()n and placed in the popup_info holding
	// object. Unless it gets stashed away in this manner, the popup
	// menu object itself will go out of scope and get destroyed, so
	// it's show()ing would be all for nothing.

	label->install_contextpopup_callback
		([popup_info=most_recent_popup::create()]
		 (ONLY IN_THREAD,
		  const x::w::element &my_element,
		  const x::w::callback_trigger_t &trigger,
		  const x::w::busy &mcguffin)
		 {
			 // We'll return immediately from the callback, but
			 // not until we grab a "wait busy" mcguffin, and
			 // and punt it to a new execution thread that
			 // waits a few seconds before creating the new
			 // popup menu and showing it.

			 x::run_lambda
				 ([popup_info, my_element,
				   mcguffin=mcguffin.get_wait_busy_mcguffin()]
				  {
					  sleep(2);

					  popup_info->created_popup=
						  create_popup_menu(my_element);
				  });
		 },

		 // Keyboard shortcut

		 {"Ctrl-F3"});
}

// New popup menu gets created, and show()n with every context popup click.

x::w::container create_popup_menu(const x::w::element &my_element)
{
	x::w::container popup_menu2=my_element->create_popup_menu
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

	// It's always our job to show() our popup. Nobody else will do it
	// for us.
	popup_menu2->show();
	return popup_menu2;
}

void popupmenu2()
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
				      "popupmenu2.examples.w.libcxx.com");
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
		popupmenu2();
	} catch (const x::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
