/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "config.h"
#include "close_flag.H"

#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/config.H>
#include <x/ref.H>
#include <x/ptr.H>
#include <x/obj.H>
#include <x/mpobj.H>

#include <x/w/main_window.H>
#include <x/w/gridlayoutmanager.H>
#include <x/w/gridfactory.H>
#include <x/w/label.H>
#include <x/w/container.H>
#include <x/w/button.H>
#include <x/w/color_picker.H>
#include <x/w/color_picker_config.H>
#include <x/w/font_picker.H>
#include <x/w/font_picker_config.H>

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>
#include <random>

// Helper object for the random color button.

// The "Random color" button callback generates a random number, and captures
// everything it needs itself. It's convenient to place the random number
// generators from the C++ libraries into a discrete object, and have the
// callback lambda capture this object.

struct random_color_sourceObj : virtual public x::obj {

	std::mt19937 rng;

	std::uniform_int_distribution<x::w::rgb_component_t
				      > dist{0, x::w::rgb::maximum};

	random_color_sourceObj()
	{
		rng.seed(std::random_device{}());
	}

	x::w::rgb random_color()
	{
		return {dist(rng), dist(rng), dist(rng)};
	}
};

typedef x::ref<random_color_sourceObj> random_color_source;

auto create_mainwindow(const x::w::main_window &main_window,
		       const x::w::screen_positions &pos)
{
	auto layout=main_window->gridlayout();

	layout->col_alignment(0, x::w::halign::right);

	layout->row_alignment(0, x::w::valign::middle);
	layout->row_alignment(1, x::w::valign::middle);

	x::w::gridfactory factory=layout->append_row();

	factory->create_label("Pick a font:");

	// create_font_picker()'s optional font_picker_config parameter.


	x::w::font_picker_config fp_config;

	// Restore previous font picker font, and most recently used fonts.

	fp_config.restore(pos, "main_font");

	// Use this to set the font picker's callback that gets invoked
	// whenever a new font gets selected.
	//
	// The font picker callback also gets invoked immediately after the
	// font picker element gets created. font_picker_config's
	// most_recently_used is a small list of font families that gets
	// listed first, in the list of available font families. The font
	// picker validates this list, and the constructor automatically
	// removes fonts that do not exist (uninstalled, etc...). The initial
	// callback invocation gives the means of retrieving the font picker's
	// finalized most recently used list.

	fp_config.callback=
		[](ONLY IN_THREAD,
		   // The new font selected in the font picker
		   const x::w::font &new_font,

		   // The new font's identifier in the dropdown list.
		   // Clearing the "selection_required" option in the
		   // font_picker_config adds an option to not have any
		   // font family selected, and a nullptr gets passed here in
		   // that case.
		   const x::w::font_picker_group_id *new_font_group,

		   // Most font picker callbacks will want to do something with
		   // themselves. Save them the trouble of weakly-capturing
		   // the font picker display element, and provide it here.
		   const x::w::font_picker &myself,

		   // The usual parameters
		   const x::w::callback_trigger_t &trigger,
		   const x::w::busy &mcguffin)
		{
			auto mru=myself->most_recently_used();

			// Initial invocation of the callback after the
			// display element gets created.
			//
			// The most recently used font list gets validated
			// during constructions. Save the validated list

			if (std::holds_alternative<x::w::initial>(trigger))
			{
				std::cout << "Restored "
					  << mru.size()
					  << " most recently used fonts"
					  << std::endl;
			}
			if (!new_font_group)
				return;

			std::cout << "New font: " << new_font
				  << std::endl;

			// Update the picker's most recently used font list.
			//
			// Start with the just-selected font

			std::vector<x::w::font_picker_group_id> new_list={
				*new_font_group
			};

			// Then add the current list of most recently used
			// font, with some adjustments:

			for (const auto &existing:mru)
			{
				// If the newly-picked font already exists
				// in the most recently used list, skip it
				// here. We already moved it to the beginning
				// of the list.

				if (existing == *new_font_group)
					continue;

				new_list.push_back(existing);

				// Keep at most three most recently used
				// fonts.
				if (new_list.size() >= 3)
					break;
			}

			// Set the new most recently used list. Both
			// IN_THREAD and non-IN_THREAD overloads are available.
			// Since we're IN_THREAD, we can use this one.
			myself->most_recently_used(IN_THREAD, new_list);

		};

	x::w::font_picker fp=factory->create_font_picker(fp_config);

	factory=layout->append_row();

	factory->create_label("Pick a color:");

	// Optional color_picker_config parameter to create_color_picker()

	// Use that to install the callback that gets invoked when a new
	// color gets selected.
	//
	// Unlike the font picker, the color picker callback does not get
	// initially invoked with trigger=x::w::initial{}, there is no
	// urgent reason to do so.

	x::w::color_picker_config cp_config{
		[](ONLY IN_THREAD,
		   const x::w::rgb &new_color,
		   const x::w::callback_trigger_t &trigger,
		   const x::w::busy &mcguffin)
		{
			std::cout << "Color picker: " << new_color
				  << std::endl;
		}};

	// Restore saved color picker's color.
	cp_config.restore(pos, "main_color");

	x::w::color_picker cp=factory->create_color_picker(cp_config);

	// A couple of buttons to set the font picker's current font.

	factory=layout->append_row();
	factory->colspan(2).halign(x::w::halign::center)
		.create_button("Normal Font")
		->on_activate
		([fp]
		 (ONLY IN_THREAD,
		  const x::w::callback_trigger_t &trigger,
		  const x::w::busy &mcguffin)
		 {
			 // Font picker's current_font() method sets a
			 // new font in the font picker. Both IN_THREAD
			 // and non-IN_THREAD overloads are available. Since
			 // this is IN_THREAD, take advantage of it.

			 fp->current_font
				 (IN_THREAD,

				  // Default-constructed font, default color
				  x::w::font{});
		 });


	factory=layout->append_row();
	factory->colspan(2).halign(x::w::halign::center)
		.create_button("Bold Font")
		->on_activate
		([fp]
		 (ONLY IN_THREAD,
		  const x::w::callback_trigger_t &trigger,
		  const x::w::busy &mcguffin)
		 {
			 x::w::font f;

			 f.set_weight("bold");
			 fp->current_font(IN_THREAD, f);
		 });

	factory=layout->append_row();
	factory->colspan(2).halign(x::w::halign::center)
		.create_button("Heading Font")
		->on_activate
		([fp]
		 (ONLY IN_THREAD,
		  const x::w::callback_trigger_t &trigger,
		  const x::w::busy &mcguffin)
		 {
			 x::w::font f;

			 f.family="liberation serif";
			 f.point_size *= 1.5;

			 fp->current_font(IN_THREAD, f);
		 });

	// Button to generate a random color

	factory=layout->append_row();
	factory->colspan(2).halign(x::w::halign::center)
		.create_button("Random color")
		->on_activate
		([cp,
		  random_color_generator=random_color_source::create()]
		 (ONLY IN_THREAD,
		  const x::w::callback_trigger_t &trigger,
		  const x::w::busy &mcguffin)
		 {
			 // Color picker's current_color() method sets a
			 // new color in the color picker. Both IN_THREAD
			 // and non-IN_THREAD overloads are available. Since
			 // this is IN_THREAD, take advantage of it.

			 cp->current_color
				 (IN_THREAD,
				  random_color_generator->random_color());
		 });

	return std::tuple{fp, cp};
}

void fontcolorpickers()
{
	// My configuration file.

	auto configfilename=
		x::configdir("fontcolorpickers@examples.w.libcxx.com")
		+ "/windows";

	auto pos=x::w::screen_positions::create(configfilename);

	x::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	x::w::font_pickerptr font_picker;
	x::w::color_pickerptr color_picker;

	x::w::main_window_config config{"main"};

	// Restore previous window positions
	config.restore(pos);

	auto main_window=x::w::main_window::create
		(config,
		 [&]
		 (const auto &main_window)
		 {
			 std::tie(font_picker, color_picker)=
				 create_mainwindow(main_window, pos);
		 });

	main_window->on_disconnect([]
				   {
					   _exit(1);
				   });

	guard(main_window->connection_mcguffin());

	main_window->set_window_title("Fonts & Colors");
	main_window->set_window_class("main",
				      "fontcolorpickers@examples.w.libcxx.com");
	main_window->on_delete
		([close_flag]
		 (ONLY IN_THREAD,
		  const x::w::busy &ignore)
		 {
			 close_flag->close();
		 });

	main_window->show_all();

	close_flag->wait();

	// Font picker's current_font() returns the most recently saved font

	std::cout << "Final font: " << font_picker->current_font()
		  << std::endl;

	// Color picker's current_color() returns the most recently
	// saved color.

	std::cout << "Final color: " << color_picker->current_color()
		  << std::endl;

	main_window->save(pos);
	pos->save(configfilename);
}

int main(int argc, char **argv)
{
	try {
		fontcolorpickers();
	} catch (const x::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
