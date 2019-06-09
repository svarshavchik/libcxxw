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
#include <string>
#include <iostream>
#include <sstream>

#include "close_flag.H"

static inline void create_main_window(const x::w::main_window &main_window)
{
	std::string me=x::exename(); // My path.
	size_t p=me.rfind('/');

	// Load "uigenerator2.xml" from the same directory as me

	x::w::const_uigenerators generator=
		x::w::const_uigenerators::create(me.substr(0, ++p) +
						 "uigenerator2.xml");

	x::w::uielements element_factory
		{
		 {
		  // Elements created from the UI theme file.

		  // Placeholders on the individual pages.

		  {"general-page-placeholder",
		   []
		   (const x::w::factory &factory)
		   {
			   factory->create_label
				   ({"serif; point_size=24"_theme_font,
				     "General Settings\n",
				     "sans_serif"_theme_font,
				     "\n\nPlaceholder\n"
				     "Placeholder\n"
				     "Placeholder\n"},
					   { x::w::halign::center});
		   },
		  },
		  {"network-page-placeholder",
		   []
		   (const x::w::factory &factory)
		   {
			   factory->create_label
				   ({"serif; point_size=24"_theme_font,
				     "Network Settings\n",
				     "sans_serif"_theme_font,
				     "\n\nPlaceholder\n"},
					   { x::w::halign::center});
		   },
		  },
		  {"local-page-placeholder",
		   []
		   (const x::w::factory &factory)
		   {
			   factory->create_label
				   ({"serif; point_size=24"_theme_font,
				     "Local Settings\n",
				     "sans_serif"_theme_font,
				     "\n\nPlaceholder\n"},
					   { x::w::halign::center});
		   },
		  },
		 },
		};

	x::w::gridlayoutmanager layout=main_window->get_layoutmanager();

	layout->generate("main-window-grid",
			 generator, element_factory);
}

void uigenerator2()
{
	x::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto main_window=x::w::main_window
		::create([&]
			 (const auto &main_window)
			 {
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

	close_flag->wait();
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
