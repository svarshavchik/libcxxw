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
#include <x/w/gridlayoutmanager.H>
#include <x/w/label.H>
#include <x/w/uielements.H>
#include <x/w/uigenerators.H>
#include <string>
#include <iostream>
#include <sstream>

#include "close_flag.H"

std::string x::appid() noexcept
{
	return "uigenerator4.examples.w.libcxx.com";
}

static inline void create_main_window(const x::w::main_window &main_window)
{
	std::string me=x::exename(); // My path.
	size_t p=me.rfind('/');

	// Load "uigenerator4.xml" from the same directory as me

	x::w::const_uigenerators generator=
		x::w::const_uigenerators::create(me.substr(0, ++p) +
						 "uigenerator4.xml");

	x::w::uielements element_factory;
	auto layout=main_window->gridlayout();

	layout->generate("main-window-grid",
			 generator, element_factory);
}

void uigenerator4()
{
	x::destroy_callback::base::guard guard;

	x::w::main_window_config config{"main"};

	auto close_flag=close_flag_ref::create();

	auto main_window=x::w::main_window
		::create(config,
			 [&]
			 (const auto &main_window)
			 {
				 create_main_window(main_window);
			 },

			 x::w::new_gridlayoutmanager{});

	main_window->set_window_title("Custom checkbox");


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

	main_window->show_all();

	close_flag->wait();
}

int main(int argc, char **argv)
{
	try {
		uigenerator4();
	} catch (const x::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
