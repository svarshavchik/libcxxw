/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "config.h"
#include <x/mpobj.H>
#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/ref.H>
#include <x/obj.H>
#include <x/appid.H>
#include <x/w/main_window.H>
#include <x/w/gridlayoutmanager.H>
#include <x/w/listlayoutmanager.H>
#include <x/w/focusable_container.H>
#include <x/w/gridfactory.H>
#include <x/w/image_param_literals.H>
#include <string>
#include <iostream>

#include "close_flag.H"

std::string x::appid() noexcept
{
	return "hierlist.examples.w.libcxx.com";
}

static const char * const lorem_ipsum[]={
	"Lorem ipsum",
	"dolor sit amet",
	"consectetur",
	"adipisicing",
	"elit",
	"sed do eiusmod",
	"tempor incididunt",
	"ut labore",
	"et dolore magna"
};

static size_t lorem_ipsum_idx=0;

// next_lorem_ipsum() always gets called from the library's internal
// execution thread (except for the initial list contents, but this doesn't
// count), so we don't need to worry about thread safety for the index
// counter. But even if there was a thread concurrency issue here this
// isn't critical for this example.

static const char *next_lorem_ipsum()
{
	size_t i=(lorem_ipsum_idx+1) %
		(sizeof(lorem_ipsum)/sizeof(lorem_ipsum[0]));

	lorem_ipsum_idx=i;

	return lorem_ipsum[i];
}


static inline void create_main_window(const x::w::main_window &main_window)
{
	auto layout=main_window->gridlayout();

	x::w::gridfactory factory=layout->append_row();

	// Configuring the list layout manager to show a hierarchical "tree".
	//
	// Started with the highlighted list style.
	x::w::new_listlayoutmanager nlm{x::w::highlighted_list};

	// The list layout manager adjusts the width of the list automatically,
	// with a fixed height. A hierarchical tree of items typically grows
	// and shrinks rather actively. Instruct the list layout manager
	// to create a fixed-sized list, 75 millimeters wide and 100 millimeters
	// tall. Scroll-bars get automatically added, as usual, when the list
	// exceeds its britches.

	nlm.width(x::w::dim_axis_arg{75});
	nlm.height(x::w::dim_axis_arg{100});

	// We'll use two columns. For a hierarchical list, the last column,
	// in this cases column #1, should have 100% of the available extra
	// space.

	nlm.columns=2;
	nlm.requested_col_widths={{1, 100}};

	// Column 0 will be a small icon. Have it aligned vertically in the
	// middle of the row.
	nlm.row_alignments={{0, x::w::valign::middle}};

	// The default horizontal padding on the left and the right of each
	// column is too big. We want column 0 to appear next to the main
	// list item, in column 1, so make the padding much smaller, .5
	// millimeters (typically 1 pixel on the left and the right, for a
	// 2 pixel padding).
	//
	// "appearance" is an object that has various settings regarding
	// the list's appearance. It is a constant, cached object, for
	// optimization purposes, an x::w::const_list_appearance.
	// To make changes to it, we use modify() to make a copy of it,
	// and adjust the temporarily-modifiable copy of the object,
	// in the closure. modify() returns a new const_list_appearance
	// that replaces the original one.

	nlm.appearance=nlm.appearance->modify
		([]
		 (const x::w::list_appearance &custom_appearance)
		 {
			 // Change our setting:
			 custom_appearance->h_padding=.5;
		 });

	// The default selection type callback visually selects the list item
	// when it's clicked on. Replace this with a custom callback. We
	// won't draw individual items as selected or unselected. Rather
	// selecting each list item either "expands" this hierachical item
	// by adding sub-items, at a higher indentation level; or removes
	// the previously-added sub-items.

	nlm.selection_type=
		[]
		(ONLY IN_THREAD,
		 const x::w::listlayoutmanager &ll,
		 size_t i,
		 const x::w::callback_trigger_t &trigger,
		 const x::w::busy &mcguffin)
		{
			// Get the indentation level of the selected item.

			size_t i_indent=ll->hierindent(i);

			// Heuristically figure out whether the list item
			// has any sub-items. Look for the following items
			// in the list. See if any list items follow this one
			// which have a higher indentation level.

			size_t s=ll->size();

			size_t e;

			for (e=i; ++e < s; )
			{
				if (ll->hierindent(e) <= i_indent)
					break;
			}

			// If there are any sub-items with a higher
			// indentation level, "collapse" this item by
			// removing those items.
			if (e-i > 1)
			{
				ll->remove_items(i+1, e-i-1);
				return;
			}

			// Add four more items after this one with an
			// increased indentation level.

			x::w::hierindent new_indent{++i_indent};

			// This is a two-column list. The first column is
			// a small image icon, the 2nd column is a random
			// text string. We also specify an indentation level
			// for each new list item.

			ll->insert_items(i+1, {
					new_indent,
					"bullet2.sxg"_image,
					next_lorem_ipsum(),
					new_indent,
					"bullet2.sxg"_image,
					next_lorem_ipsum(),
					new_indent,
					"bullet2.sxg"_image,
					next_lorem_ipsum(),
					new_indent,
					"bullet2.sxg"_image,
					next_lorem_ipsum()
				});
		};

	factory->create_focusable_container
		([]
		 (const auto &fc)
		 {
			 // Initial contents of the list, the initial
			 // item, the top of the fake tree.

			 auto ll=fc->listlayout();

			 ll->append_items({"bullet2.sxg"_image,
					   lorem_ipsum[0]});
		 },
		 nlm);
}

void hierlist()
{
	x::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto main_window=x::w::main_window
		::create([&]
			 (const auto &main_window)
			 {
				 create_main_window(main_window);
			 });

	main_window->set_window_title("Hierarchical list");
	main_window->set_window_class("main",
				      "hierlist.examples.w.libcxx.com");

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
		hierlist();
	} catch (const x::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
