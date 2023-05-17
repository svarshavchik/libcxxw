/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "config.h"
#include <x/mpobj.H>
#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/pidinfo.H>
#include <x/config.H>
#include <x/appid.H>
#include <x/w/main_window.H>
#include <x/w/menubarlayoutmanager.H>
#include <x/w/listlayoutmanager.H>
#include <x/w/listitemhandle.H>
#include <x/w/uielements.H>
#include <x/w/uigenerators.H>
#include <x/w/menu.H>
#include <x/w/copy_cut_paste_menu_items.H>
#include <x/w/element_state.H>
#include <x/w/callback_trigger.H>
#include <string>
#include <iostream>
#include <sstream>

#include "close_flag.H"

std::string x::appid() noexcept
{
	return "uigenerator6.examples.w.libcxx.com";
}

static inline void create_main_window(const x::w::main_window &main_window,
				      const close_flag_ref &close_flag)
{
	std::string me=x::exename(); // My path.
	size_t p=me.rfind('/');

	// Load "uigenerator6.xml" from the same directory as me

	x::w::const_uigenerators generator=
		x::w::const_uigenerators::create(me.substr(0, ++p) +
						 "uigenerator6.xml");

	x::w::uielements element_factory;

	// Set "main" element to the main window.
	//
	// <append_copy_cut_paste> specifies that its attached element is
	// "main".
	element_factory.new_elements.emplace("main", main_window);

	auto layout=main_window->gridlayout();

	layout->generate("main-window-grid",
			 generator, element_factory);

	// Get the menu bar, and generate() its contents.
	x::w::menubarlayoutmanager mb=
		main_window->get_menubarlayoutmanager();

	element_factory.list_item_status_change_callbacks.emplace(
		"help_about_callback",
		[]
		(ONLY IN_THREAD,
		 const x::w::list_item_status_info_t &i)
		{
			if (std::holds_alternative<x::w::initial>(i.trigger))
				return;

			std::cout << "Help/About" << std::endl;
		}
	);

	mb->generate("main-window-menu", generator, element_factory);

	// Attach callbacks for the menu items.

	x::w::listitemhandle h=element_factory.get_listitemhandle("file_new");

	h->on_status_update
		([]
		 (ONLY IN_THREAD,
		  const x::w::list_item_status_info_t &i)
		 {
			 if (std::holds_alternative<x::w::initial>(i.trigger))
				 return;

			 std::cout << "File/New" << std::endl;
		 });

	h=element_factory.get_listitemhandle("file_open");

	h->on_status_update
		([]
		 (ONLY IN_THREAD,
		  const x::w::list_item_status_info_t &i)
		 {
			 if (std::holds_alternative<x::w::initial>(i.trigger))
				 return;

			 std::cout << "File/Open" << std::endl;
		 });

	h=element_factory.get_listitemhandle("file_close");

	h->on_status_update
		([close_flag]
		 (ONLY IN_THREAD,
		  const x::w::list_item_status_info_t &i)
		 {
			 if (std::holds_alternative<x::w::initial>(i.trigger))
				 return;

			 close_flag->close();
		 });

	h=element_factory.get_listitemhandle("file_recent_file_1");

	h->on_status_update
		([]
		 (ONLY IN_THREAD,
		  const x::w::list_item_status_info_t &i)
		 {
			 if (std::holds_alternative<x::w::initial>(i.trigger))
				 return;

			 std::cout << "File/Recent/1" << std::endl;
		 });

	h=element_factory.get_listitemhandle("file_recent_file_2");

	h->on_status_update
		([]
		 (ONLY IN_THREAD,
		  const x::w::list_item_status_info_t &i)
		 {
			 if (std::holds_alternative<x::w::initial>(i.trigger))
				 return;

			 std::cout << "File/Recent/2" << std::endl;
		 });

	h=element_factory.get_listitemhandle("file_automatic_close");

	h->on_status_update
		([]
		 (ONLY IN_THREAD,
		  const x::w::list_item_status_info_t &i)
		 {
			 if (std::holds_alternative<x::w::initial>(i.trigger))
				 return;

			 std::cout << "File/Automatic Close: "
				   << i.selected << std::endl;
		 });

	h=element_factory.get_listitemhandle("file_plain_format");

	h->on_status_update
		([]
		 (ONLY IN_THREAD,
		  const x::w::list_item_status_info_t &i)
		 {
			 if (std::holds_alternative<x::w::initial>(i.trigger))
				 return;

			 std::cout << "File/Plain Format: "
				   << i.selected << std::endl;
		 });

	h=element_factory.get_listitemhandle("file_full_format");
	h->on_status_update
		([]
		 (ONLY IN_THREAD,
		  const x::w::list_item_status_info_t &i)
		 {
			 if (std::holds_alternative<x::w::initial>(i.trigger))
				 return;

			 std::cout << "File/Full Format: "
				   << i.selected << std::endl;
		 });

	// In the theme file:
	//
	// <add id="file_menu"> adds the "File" menu.
	//
	// Before showing the file menu update() the copy/cut/paste menu items.

	x::w::copy_cut_paste_menu_items ccp=
		element_factory.new_copy_cut_paste_menu_items;

	x::w::menu file_menu=element_factory.get_element("file_menu");

	file_menu->on_popup_state_update
		([ccp]
		 (ONLY IN_THREAD,
		  const x::w::element_state &state,
		  const x::w::busy &mcguffin)
		 {
			 if (state.state_update == state.before_showing)
			 {
				 ccp->update();
			 }
		 });
}

void uigenerator6()
{
	x::destroy_callback::base::guard guard;

	x::w::main_window_config config{"main"};

	auto close_flag=close_flag_ref::create();

	auto main_window=x::w::main_window
		::create(config,
			 [&]
			 (const auto &main_window)
			 {
				 create_main_window(main_window, close_flag);
			 },

			 x::w::new_gridlayoutmanager{});

	main_window->set_window_title("List");


	guard(main_window->connection_mcguffin());

	main_window->on_disconnect([]
				   {
					   _exit(1);
				   });

	main_window->on_delete
		([close_flag]
		 (ONLY IN_THREAD,
		  const x::w::busy &ignore)
		 {
			 close_flag->close();
		 });

	// Show the menu bar, and the window
	main_window->get_menubar()->show();
	main_window->show_all();

	close_flag->wait();
}

int main(int argc, char **argv)
{
	try {
		uigenerator6();
	} catch (const x::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
