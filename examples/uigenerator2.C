/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "config.h"
#include <x/mpobj.H>
#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/pidinfo.H>
#include <x/w/main_window.H>
#include <x/w/gridlayoutmanager.H>
#include <x/w/label.H>
#include <x/w/uielements.H>
#include <x/w/uigenerators.H>
#include <x/w/text_param_literals.H>
#include <x/w/progressbar.H>
#include <string>
#include <iostream>
#include <sstream>
#include <tuple>
#include <chrono>

#include "close_flag.H"


static inline auto create_main_window(const x::w::main_window &main_window)
{
	std::string me=x::exename(); // My path.
	size_t p=me.rfind('/');

	// Load "uigenerator2.xml" from the same directory as me

	x::w::const_uigenerators generator=
		x::w::const_uigenerators::create(me.substr(0, ++p) +
						 "uigenerator2.xml");

	x::w::uielements element_factory;

	x::w::gridlayoutmanager layout=main_window->get_layoutmanager();

	layout->generate("main-window-grid",
			 generator, element_factory);

	// generate() saved the id-ed widgets that were created. Fish out
	// the progressbar widget and its label widget, and return them.

	return std::tuple{
		element_factory.get_element("progressbar_element"),
			element_factory.get_element("progressbar_label")
			};
}

void uigenerator2()
{
	x::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	x::w::progressbarptr progressbar;
	x::w::labelptr progressbar_label;

	auto main_window=x::w::main_window
		::create([&]
			 (const auto &main_window)
			 {
				 std::tie(progressbar, progressbar_label)=
					 create_main_window(main_window);
			 },

			 x::w::new_gridlayoutmanager{});

	main_window->set_window_class("main",
				      "uigenerator2@examples.w.libcxx.com");

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

	// While waiting for the window to be closed, update the progress bar.
	x::mpcobj<bool>::lock lock{close_flag->flag};

	int n=0;

	do
	{
		progressbar->update(n, 100,
				    [=]
				    (ONLY IN_THREAD)
				    {
					    std::ostringstream o;

					    o << n << "%";

					    progressbar_label->update(o.str());
				    });
		n= (n+10)%110;

		lock.wait_for(std::chrono::milliseconds(500),
			      [&] { return *lock; });
	} while (!*lock);
}

int main(int argc, char **argv)
{
	try {
		uigenerator2();
	} catch (const x::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
