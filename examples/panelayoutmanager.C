/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "config.h"
#include <x/mpobj.H>
#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/ref.H>
#include <x/obj.H>
#include <x/w/main_window.H>
#include <x/w/gridlayoutmanager.H>
#include <x/w/gridfactory.H>
#include <x/w/focusable_container.H>
#include <x/w/panelayoutmanager.H>
#include <x/w/panefactory.H>
#include <x/w/button.H>
#include <x/w/label.H>
#include <x/w/standard_comboboxlayoutmanager.H>
#include <x/w/listlayoutmanager.H>
#include <x/w/canvas.H>

#include <string>
#include <iostream>
#include <sstream>

#include "close_flag.H"
#include "panelayoutmanager.H"

static void insert(const x::w::container &c,
		   x::w::scrollbar_visibility v);
static void append(const x::w::container &c,
		   x::w::scrollbar_visibility v);
static void remove_first(const x::w::container &c);
static void remove_last(const x::w::container &c);
static void replace_first(const x::w::container &c,
			  x::w::scrollbar_visibility v);
static void insert_list(const x::w::container &c);
static void replace_all(const x::w::container &c,
			  x::w::scrollbar_visibility v);

// Translate the currently selected item in the scrollbar visibility combo-box
// to an x::w::scrollbar_visibility value.

static x::w::scrollbar_visibility
get_scrollbar_visibility(const x::w::container &container)
{
	x::w::standard_comboboxlayoutmanager lm=
		container->get_layoutmanager();

	auto selected=lm->selected();

	size_t i=selected ? *selected:0;

	static const x::w::scrollbar_visibility values[]={
		x::w::scrollbar_visibility::never,
		x::w::scrollbar_visibility::always,
		x::w::scrollbar_visibility::automatic,
		x::w::scrollbar_visibility::automatic_reserved
	};

	return values[i];
}

static void create_main_window(const x::w::main_window &mw,
			       const options &opts)
{
	// Create a container that uses the pane layout manager.
	//
	// x::w::new_panelayoutmanager's constructor takes an x::w::dim_arg
	// parameter, that specifies the container's size.
	//
	// The minimum size is 10mm, the default (initial) size is 50,
	// and the maximum size is 100 mm:

	x::w::new_panelayoutmanager npl{{10,50,100}};

	if (opts.horizontal->value)
		npl.horizontal();

	x::w::gridlayoutmanager layout=mw->get_layoutmanager();
	auto factory=layout->append_row();

	auto pane=factory->colspan(2)
		.halign(x::w::halign::fill)
		.create_focusable_container
		([]
		 (const auto &pane_container) {
			 // Initially empty
			 //
			 // In most cases the panes are predetermined, and
			 // they'll get initialized here.
		}, npl);

	pane->show();

	// Create a combo-box for choosing the visibility of the next
	// created pane's scroll-bar.

	factory=layout->append_row();

	factory->halign(x::w::halign::right)
		.valign(x::w::valign::bottom)
		.create_label("New elements'\nscrollbar visibility:")->show();

	auto scrollbar_visibility=
		factory->valign(x::w::valign::bottom)
		.create_focusable_container
		([&]
		 (const auto &c)
		 {
			 x::w::standard_comboboxlayoutmanager lm=
			 c->get_layoutmanager();

			 lm->append_items({ "Hide",
					 "Always",
					 "When needed",
					 "Reserve space"});

			 lm->selected(3, true);
		 },
		 x::w::new_standard_comboboxlayoutmanager{});

	scrollbar_visibility->show();

	// Create buttons that perform various pane operations.

	factory=layout->append_row();

	factory->halign(x::w::halign::left);
	auto b=factory->create_normal_button_with_label("Insert");

	b->show();
	b->on_activate([pane, scrollbar_visibility]
		       (ONLY IN_THREAD,
			const auto &trigger,
			const auto &busy) {
			       insert(pane, get_scrollbar_visibility
				      (scrollbar_visibility));
		       });

	factory->halign(x::w::halign::right);
	b=factory->create_normal_button_with_label("Append");

	b->show();
	b->on_activate([pane, scrollbar_visibility]
		       (ONLY IN_THREAD,
			const auto &trigger,
			const auto &busy) {
			       append(pane, get_scrollbar_visibility
				      (scrollbar_visibility));
		       });

	factory=layout->append_row();

	factory->halign(x::w::halign::left);
	b=factory->create_normal_button_with_label("Remove 1st pane");

	b->show();
	b->on_activate([pane]
		       (ONLY IN_THREAD,
			const auto &trigger,
			const auto &busy) {
			       remove_first(pane);
		       });

	factory->halign(x::w::halign::right);
	b=factory->create_normal_button_with_label("Remove last pane");

	b->show();
	b->on_activate([pane]
		       (ONLY IN_THREAD,
			const auto &trigger,
			const auto &busy) {
			       remove_last(pane);
		       });

	factory=layout->append_row();

	factory->halign(x::w::halign::left);
	b=factory->create_normal_button_with_label("Replace 1st pane");

	b->show();
	b->on_activate([pane, scrollbar_visibility]
		       (ONLY IN_THREAD,
			const auto &trigger,
			const auto &busy) {
			       replace_first(pane, get_scrollbar_visibility
					     (scrollbar_visibility));
		       });

	factory->halign(x::w::halign::right);
	b=factory->create_normal_button_with_label("Remove all panes");

	b->show();
	b->on_activate([pane]
		       (ONLY IN_THREAD,
			const auto &trigger,
			const auto &busy) {
			       	x::w::panelayoutmanager
					lm=pane->get_layoutmanager();

				lm->remove_all_panes();
		       });

	factory=layout->append_row();

	// The sample UI for lists in panes doesn't work quite well with
	// horizontal panes. It's designed for vertical panes.

	if (opts.horizontal->value)
	{
		// Empty spacer in its place.
		factory->create_canvas()->show();
	}
	else
	{
		factory->halign(x::w::halign::left);
		b=factory->create_normal_button_with_label("Insert list");

		b->show();
		b->on_activate([pane]
			       (ONLY IN_THREAD,
				const auto &trigger,
				const auto &busy) {
				       insert_list(pane);
			       });
	}
	factory->halign(x::w::halign::right);

	b=factory->create_normal_button_with_label("Replace All");

	b->show();
	b->on_activate([pane, scrollbar_visibility]
		       (ONLY IN_THREAD,
			const auto &trigger,
			const auto &busy) {
			       replace_all(pane, get_scrollbar_visibility
					   (scrollbar_visibility));
		       });

}

static void insert(const x::w::container &c,
		   x::w::scrollbar_visibility v)
{
	x::w::panelayoutmanager lm=c->get_layoutmanager();

	// insert_panes() returns a factory that creates new panes before
	// an existing pane. Create a new pane before the first existing
	// pane.
	x::w::panefactory f=lm->insert_panes(0);

	// The factory's set_initial_size() specifies the new element's
	// "virtual" size, in millimeters:

	f->set_initial_size(20);

	// The actual size of the pane container gets set when the pane
	// layout manager's container gets created. It's given as the
	// parameter to x::w::new_panelayoutmanager's constructor, and the
	// size of the container does not vary outside of this preset size.
	//
	// Adding a new element to the container only expands the container
	// up to its maximum size. If it exceeds it, all existing elements
	// in the pane container get resized proportionately to their existing
	// size and the size specified by set_initial_size().
	//
	// So, for example, if the pane container is full and its already
	// at its maximum size of 20 millimeters, and this new element's
	// 20 millimeter makes the new total size of 40, and all elements,
	// including the new element's size gets cut in half since the pane's
	// size is limited to 20 mm total.


	// The factory's set_initial_size() sets the initial size of the
	// new page. set_scrollbar_visibility() specifies when and how the
	// pane's scroll-bar is visible. This, and other, options get set
	// before creating a new display element, which then becomes the
	// new pane.
	//
	// Repeatedly using the same factory to create multiple display
	// elements creates a new pane for each one; however all pane
	// properties get reset back to their default values, and must be
	// explicitly set before creating each new pane.

	f->set_scrollbar_visibility(v)
		.create_label("Lorem ipsum\n"
			      "dolor sit amet\n"
			      "consectetur\n"
			      "adipisicing elit sed\n"
			      "do eiusmod\n"
			      "tempor incididunt ut\n"
			      "labore et\n"
			      "dolore magna\n"
			      "aliqua");

	// We know that the label is element #0. get() returns the existing
	// element in the pane container. size(), see below, returns the number
	// of elements in the container.

	x::w::label l=lm->get(0);

	// create_label() created a new label. Like all display elements
	// it's necessary to show() it, in order to make it visible.
	//
	// This song and dance routine is just for demonstration purposes.
	// ->show()-ing the returned create_label() is sufficient.

	l->show();
}

static void append(const x::w::container &c,
		   x::w::scrollbar_visibility v)
{
	x::w::panelayoutmanager lm=c->get_layoutmanager();

	// append_factory() returns a factory that adds a new pane after all
	// existing panes, if any.
	x::w::panefactory f=lm->append_panes();

	// The set_background_color() uses a new background color for the
	// new pane, either an explicit x::w::rgb value, or a name of a theme-
	// specified color.
	//
	// padding() uses non-default padding for the new pane.

	f->set_background_color("100%")
		.padding(2.0)
		.set_scrollbar_visibility(v);

	f->create_label("Lorem ipsum "
			"dolor sit amet\n"
			"consectetur "
			"adipisicing elit sed\n"
			"do eiusmod "
			"tempor incididunt ut\n"
			"labore et "
			"dolore magna\n"
			"aliqua")->show();
}

// remove_pane() removes an existing pane from the contanier.

static void remove_first(const x::w::container &c)
{
	x::w::panelayoutmanager lm=c->get_layoutmanager();

	if (lm->size() > 0)
		lm->remove_pane(0);
}

static void remove_last(const x::w::container &c)
{
	x::w::panelayoutmanager lm=c->get_layoutmanager();

	auto s=lm->size();

	if (s > 0)
		lm->remove_pane(s-1);
}

static void replace_first(const x::w::container &c,
			  x::w::scrollbar_visibility v)
{
	x::w::panelayoutmanager lm=c->get_layoutmanager();

	// replace_panes() returns a factory that replaces an existing
	// pane with the new pane, rather than inserting or appending a new
	// pane.
	//
	// The sliders in the container adjust the relative sizes of the
	// panes on their sides. The pane sizes are constrained only by
	// the sizes of the entire container, and it's possible for a pane
	// to grow larger than the display element in the pane.
	//
	// Additionally, the width of a vertical pane (and the height
	// of a horizontal pane) gets sized to accomodate the widest/tallest
	// pane.
	//
	// valign() and halign() controls the position of the new element in
	// its pane, when there's extra space for it.

	x::w::panefactory f=lm->replace_panes(0);

	f->set_background_color("100%")
		.padding(2.0)
		.set_scrollbar_visibility(v)
		.valign(x::w::valign::bottom);

	f->create_label("Lorem ipsum "
			"dolor sit amet\n"
			"consectetur "
			"adipisicing elit sed\n"
			"do eiusmod "
			"tempor incididunt ut\n"
			"labore et "
			"dolore magna\n"
			"aliqua")->show();
}

static void replace_all(const x::w::container &c,
			  x::w::scrollbar_visibility v)
{
	x::w::panelayoutmanager lm=c->get_layoutmanager();

	// replace_all_panes() is equivalent to calling
	// the pane layout manager's remove_all_panes()
	// followed by append_panes().

	x::w::panefactory f=lm->replace_all_panes();

	for (int i=0; i<2; ++i)
	{
		f->set_background_color("100%")
			.padding(2.0)
			.set_scrollbar_visibility(v)
			.create_label("Lorem ipsum "
				      "dolor sit amet\n"
				      "consectetur "
				      "adipisicing elit sed\n"
				      "do eiusmod "
				      "tempor incididunt ut\n"
				      "labore et "
				      "dolore magna\n"
				      "aliqua")->show();
	}

}


// Focusable containers get automatically incorporated into the pane's
// internal tabbing order of its sliders and scroll-bars. This is especially
// useful with selection lists.
//
// The "insert list" button is useful only with selection lists.

static void insert_list(const x::w::container &c)
{
	x::w::panelayoutmanager lm=c->get_layoutmanager();

	x::w::panefactory f=lm->insert_panes(0);

	// Each pane element can be a container, containing other elements.
	//
	// Create a focusable container with a selection list.

	x::w::new_listlayoutmanager nlm;

	// Update both the pane factory's settings and the new list
	// layout manager settings for optimal results for a list in
	// a container with the pane layout manager.
	//
	// configure_new_list()'s parameter is either an
	// x::w::new_listlayoutmanager or an x::w::new_tablelayoutmanager.

	f->configure_new_list(nlm);

	f->set_initial_size(20)
		.create_focusable_container
		([]
		 (const auto &container)
		 {
			 x::w::listlayoutmanager lm=
				 container->get_layoutmanager();

			 lm->append_items({
					 "Lorem ipsum",
					 "dolor sit amet",
					 "consectetur",
					 "adipisicing elit sed",
					 "do eiusmod",
					 "tempor incididunt ut",
					 "labore et",
					 "dolore magna",
					 "aliqua"});
		 },
		 nlm)->show();
}

void testpane(const options &opts)
{
	x::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto main_window=x::w::main_window
		::create([&]
			 (const auto &mw)
			 {
				 create_main_window(mw, opts);
			 });

	main_window->set_window_title("Panes!");
	main_window->set_window_class("main", "testpane@examples.w.libcxx.com");

	guard(main_window->connection_mcguffin());

	main_window->on_disconnect([]
				   {
					   exit(1);
				   });

	main_window->on_delete
		([close_flag]
		 (ONLY IN_THREAD,
		  const auto &ignore)
		 {
			 close_flag->close();
		 });

	main_window->show();

	x::mpcobj<bool>::lock lock{close_flag->flag};
	lock.wait([&] { return *lock; });
}

int main(int argc, char **argv)
{
	try {
		options opts;

		opts.parse(argc, argv);

		testpane(opts);
	} catch (const x::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
