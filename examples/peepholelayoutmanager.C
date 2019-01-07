/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "config.h"
#include "close_flag.H"

#include <x/exception.H>
#include <x/destroy_callback.H>

#include <x/w/main_window.H>
#include <x/w/peepholelayoutmanager.H>
#include <x/w/gridlayoutmanager.H>
#include <x/w/gridfactory.H>
#include <x/w/container.H>
#include <x/w/focusable_container.H>
#include <x/w/button.H>
#include <x/w/canvas.H>
#include <x/w/text_param_literals.H>
#include <x/w/shortcut.H>

#include <iostream>
#include <sstream>

// Peepholed contents. Both peepholes use the grid layout manager.

// Create a single row of ten buttons.
void create_peepholed_buttons(const x::w::container &c)
{
	x::w::gridlayoutmanager glm=c->get_layoutmanager();

	auto f=glm->append_row();

	for (int i=0; i<10; ++i)
	{
		std::ostringstream o;

		o << "Button " << (char)('A'+i);

		f->create_normal_button_with_label(o.str());
	}
}

// Peephole layout manager callback. This gets passed to
// new_scrollable_peepholelayoutmanager or new_peepholelayoutmanager's
// constructor, below, to create the peepholed contents.

void create_peepholed_container(const x::w::factory &f)
{
	f->create_container(create_peepholed_buttons,
			    x::w::new_gridlayoutmanager{});
}

// The contents of the main window.
void create_peepholes(const x::w::main_window &mw)
{
	x::w::gridlayoutmanager glm=mw->get_layoutmanager();

	// Two rows, one peephole on each row.

	auto f=glm->append_row();

	// Ample padding.

	f->padding(20);

	x::w::new_scrollable_peepholelayoutmanager
		nsplm{create_peepholed_container};

	// Peephole's width.
	nsplm.width({20, 100, 300});

	f->colspan(2).create_focusable_container
		([]
		 (const x::w::focusable_container &c)
		 {
			 // Unused
		 },
		 nsplm);

	// A row for the 2nd peephole, also padded.

	f=glm->append_row();

	f->padding(20);

	x::w::new_peepholelayoutmanager nplm{create_peepholed_container};

	nplm.width({20, 100, 300});

	// This is a plain peephole, no scroll-bars. Help things along by
	// centering the display element with the keyboard focus, if its
	// tabbed to.
	nplm.scroll=x::w::peephole_scroll::centered;

	auto bottom_peephole=
		f->colspan(2).create_container([]
					       (const x::w::container &c)
					       {
						       // Unused
					       },
					       nplm);

	// The "Flip" button on the last row, with an ALT-F shortcut.

	f=glm->append_row();
	auto flip_button=f->halign(x::w::halign::center)
		.left_padding(20)
		.create_special_button_with_label
		({
		  "underline"_decoration,
		  "F",
		  "no"_decoration,
		  "lip"
		},
			{"Alt", 'F'});

	// The flip button is not a direct parent or child of the
	// buttom_peephole, so its callback can safely capture it by value.

	flip_button->on_activate
		([bottom_peephole, which=0]
		 (ONLY IN_THREAD,
		  const x::w::callback_trigger_t &trigger,
		  const x::w::busy &mcguffin)
		 mutable
		 {
			 // Clicking on the Flip button alternatively moves
			 // the bottom_peephole's first and last button
			 // into view.
			 //
			 // First, get the peephole layout manager.

			 x::w::peepholelayoutmanager plm=
				 bottom_peephole->get_layoutmanager();

			 // The peepholed container, that uses the grid
			 // layout manager.

			 x::w::container c=plm->get();

			 x::w::gridlayoutmanager glm=c->get_layoutmanager();

			 // The buttons are all in row 0.
			 //
			 // ensure_entire_visibility() of the first or the
			 // last button.
			 x::w::button b=glm->get(0, which);

			 b->ensure_entire_visibility(IN_THREAD);

			 // The next time flip_button gets clicked on,
			 // it will ensure_entire_visibility() of the other
			 // button.
			 which=glm->cols(0)-1-which;
		 });

	// The peepholes specify a minimum, preferred, and maximum size.
	//
	// Having just the reset button on the last row would constrain
	// the peepholes' width. The reset button is small, and doesn't
	// stretch, so with the mere button alone the grid layout manager
	// will limit the width of the grid because of the reset button's
	// fixed width. It's in the same column as the peephole, so the
	// grid layout manager sees that the reset button can't get any
	// wider, and that's that.
	//
	// However we want to be able to resize the peepholes, by resizing
	// the main window. We do this by creating an empty canvas
	// next to the reset button that can stretch, and making the
	// two peepholes span both columns. Now, since the canvas can
	// absorb the extra space, the grid layout manager factors that in,
	// and allows the grid to expand to the maximum width of the
	// peepholes, since the peepholes span both columns, and the canvas's
	// maximum width can expand too.

	f->create_canvas([]
			 (const auto &ignore)
			 {
			 },

			 // A plain create_canvas() sets its preferred width
			 // to 0 pixels. In order to compute the initial
			 // default width of the grid we need to make
			 // the canvas's preferred width the same 100
			 // millimeters as the peepholes'.
			 //
			 // So we need to use the overloaded create_canvas()
			 // that explicitly sets the canvas's metrics:
			 {0, 100},
			 {0, 0, 0});
}

void peepholelayoutmanager()
{
	x::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto main_window=x::w::main_window::create(create_peepholes);

	main_window->set_window_title("Peepholes");

	guard(main_window->connection_mcguffin());

	main_window->on_disconnect([]
				   {
					   _exit(1);
				   });

	main_window->on_delete
		([close_flag]
		 (THREAD_CALLBACK,
		  const auto &ignore)
		 {
			 close_flag->close();
		 });

	main_window->show_all();

	x::mpcobj<bool>::lock lock{close_flag->flag};

	lock.wait([&] { return *lock; });
}

int main(int argc, char **argv)
{
	try {
		peepholelayoutmanager();
	} catch (const x::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
