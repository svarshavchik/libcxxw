/*
** Copyright 2019-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "config.h"
#include <x/mpobj.H>
#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/pidinfo.H>
#include <x/config.H>
#include <x/w/main_window.H>
#include <x/w/gridlayoutmanager.H>
#include <x/w/label.H>
#include <x/w/uielements.H>
#include <x/w/uigenerators.H>
#include <x/w/text_param_literals.H>
#include <x/w/font_picker.H>
#include <x/w/progressbar.H>
#include <x/w/screen_positions.H>
#include <string>
#include <iostream>
#include <sstream>
#include <tuple>
#include <chrono>

#include "close_flag.H"


static inline auto create_main_window(const x::w::main_window &main_window,
				      const x::w::screen_positions &pos)
{
	std::string me=x::exename(); // My path.
	size_t p=me.rfind('/');

	// Load "uigenerator2.xml" from the same directory as me

	x::w::const_uigenerators generator=
		x::w::const_uigenerators::create(me.substr(0, ++p) +
						 "uigenerator2.xml",

						 // Previous settings of the
						 // font and color picker
						 // widgets to restore:

						 pos);

	x::w::uielements element_factory;

	auto layout=main_window->gridlayout();

	layout->generate("main-window-grid",
			 generator, element_factory);

	// Retrieve the generated font picker widget.
	//
	// Install a bare font selection callback that updates the font
	// picker's most-recently-used fonts to the last three selected
	// fonts.

	x::w::font_picker fp=element_factory.get_element("default-font");

	fp->on_font_update
		([]
		 (ONLY IN_THREAD,
		  const x::w::font &new_font,
		  const x::w::font_picker_group_id *new_font_group,
		  const x::w::font_picker &myself,
		  const x::w::callback_trigger_t &trigger,
		  const x::w::busy &mcguffin)
		{
			if (!new_font_group)
				return;

			auto mru=myself->most_recently_used();

			std::vector<x::w::font_picker_group_id> new_list;

			new_list.reserve(3);

			new_list.push_back(*new_font_group);

			for (const auto &existing:mru)
			{
				if (existing == *new_font_group)
					continue;

				new_list.push_back(existing);

				if (new_list.size() >= 3)
					break;
			}

			myself->most_recently_used(IN_THREAD, new_list);
		});

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

	// Configuration filename where we save the window's position.

	std::string configfile=
		x::configdir("uigenerator2@examples.w.libcxx.com")
		+ "/windows";

	x::w::main_window_config config;

	auto pos=x::w::screen_positions::create(configfile);

	config.restore(pos, "main");

	auto close_flag=close_flag_ref::create();

	x::w::progressbarptr progressbar;
	x::w::labelptr progressbar_label;

	auto main_window=x::w::main_window
		::create(config,
			 [&]
			 (const auto &main_window)
			 {
				 std::tie(progressbar, progressbar_label)=
					 create_main_window(main_window, pos);
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

	main_window->save(pos);
	pos->save(configfile);
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
