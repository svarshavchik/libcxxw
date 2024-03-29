/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "config.h"
#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/refptr_traits.H>
#include <x/weakcapture.H>
#include <x/appid.H>

#include <x/w/main_window.H>
#include <x/w/gridlayoutmanager.H>
#include <x/w/gridfactory.H>
#include <x/w/label.H>
#include <x/w/button.H>
#include <x/w/image_button.H>
#include <string>
#include <iostream>

#include "gridlayoutmanager.inc.H"
#include "close_flag.H"

std::string x::appid() noexcept
{
	return "helloworld.examples.w.libcxx.com";
}

// This is attached as the main_window's appdata.

// Inherits from the stylesheet-generated list of display elements, defined
// in gridlayoutmanager.xml

class appdataObj : virtual public x::obj,
		   public mainwindowfields {

public:

	using mainwindowfields::mainwindowfields;
};

typedef x::ref<appdataObj> appdata;

static void update_button_state(const appdata &my_appdata);

// When the insert button gets clicked we insert a
// new label in the horizontal_container, at the beginning of the row,
// pushing all other elements to the right.

static inline void insert_column(const appdata &my_appdata,
				 int counter)
{
	std::ostringstream o;

	o << counter;

	auto l=my_appdata->horizontal_container->gridlayout();

	// First time, there are no rows, so append one. Otherwise insert
	// the new label at the beginning of the row.

	auto factory=l->rows() == 0
		? l->append_row()
		: l->insert_columns(0, 0);

	factory->create_label(o.str())->show();
	update_button_state(my_appdata);
}

// And the "Remove Column" button removes the last label in the horizontal
// container.

static inline void remove_column(const appdata &my_appdata)
{
	auto l=my_appdata->horizontal_container->gridlayout();

	if (l->rows() == 0)
		return; // No first row.

	auto s=l->cols(0);

	l->remove(0, --s);

	if (s == 0)
		l->remove_row(0);
	update_button_state(my_appdata);
}

// When the insert button gets clicked we insert a new row #0 into the
// vertical container, pushing its existing rows down.
//
// The new row consists of a checkbox and a label.

static inline void insert_row(const appdata &my_appdata,
			      int counter)
{
	std::ostringstream o;

	o << counter;

	// The main window's grid layout manager.

	auto l=my_appdata->vertical_container->gridlayout();

	// So, we're inserting row #1.

	auto factory=l->insert_row(0);

	auto checkbox=
		factory->create_checkbox([&]
					 (const auto &factory)
					 {
						 factory->create_label(o.str()) ->show();
					 });

	// One last detail before we can show. The tabbing order of focusable
	// fields is the order they're created in, irrespective of their
	// actual position on the window. Therefore,

	if (l->rows() == 1)
		// This is the first checkbox. Set it's tabbing order after
		// the "Remove Row" button.
		checkbox->get_focus_after(my_appdata->remove_row);
	else
	{
		// This is not the first checkbox. Set it's tabbing order
		// below the one that just got bumped down.
		//
		// We can simply do the same thing, after the remove row
		// button, but we want to show off, here.

		x::w::image_button other=l->get(1, 0);

		checkbox->get_focus_before(other);
	}

	checkbox->show();
	update_button_state(my_appdata);
}

// And the "Remove Row" button removes the bottom-most row.

static inline void remove_row(const appdata &my_appdata)
{
	auto l=my_appdata->vertical_container->gridlayout();

	if (l->rows() == 0)
		return; // No row.

	l->remove_row(l->rows()-1);
	update_button_state(my_appdata);
}

// Ok, so we'll enable the remove_column() button only if there are columns
// to remove, and remove_row() only if there are rows to remove.
//
// This does make checking if there's anything to remove (in remove_row() and
// remove_column()) redundant. We'll never be able to get there, in that case.
// Still that's a proper thing to do.

static void update_button_state(const appdata &my_appdata)
{
	auto l=my_appdata->horizontal_container->gridlayout();

	my_appdata->remove_column->set_enabled(l->rows() && l->cols(0));

	l=my_appdata->vertical_container->get_layoutmanager();

	my_appdata->remove_row->set_enabled(l->rows() > 0);
}

// Main window's creator.

inline void create_mainwindow(const x::w::main_window &main_window)
{
	mainwindowfieldsptr fields;

	auto layout=main_window->gridlayout();

	// The main window is divided into three parts. The top part
	// will have elements that get added horizontally across, with
	// columns being added or removed.
	//
	// The middle part will have rows added and removed. These rows
	// have two columns, a checkbox and a label.
	//
	// Finally, the bottom part of the window has a bunch of buttons,
	// arranged horizontally in columns.
	//
	// The columns in the three parts are not tied to each other, but
	// a single grid layout manager expects to have the same number of
	// columns in each row, which it will line up evenly.
	//
	// So, what we do is create nested containers, one for the top,
	// one for the middle, and one for the bottom part, and main window's
	// grid layout manager has just a single column to deal with, with
	// the nested grid layout managers lining up the columns in their
	// respective domains.

	fields.horizontal_container=layout->append_row()
		->create_container([]
				   (const auto &)
				   {
					   // Nothing to add, for now
				   },
				   x::w::new_gridlayoutmanager());

	fields.vertical_container=layout->append_row()->create_container
		([]
		 (const auto &container)
		 {
		 },
		 x::w::new_gridlayoutmanager());

	// The bottom part is the container with all the buttons.

	layout->append_row()->create_container
		([&]
		 (const auto &button_row)
		 {
			 // The creator for the button row container. Create
			 // the buttons.

			 auto layout=button_row->gridlayout();

			 auto factory=layout->append_row();

			 fields.insert_column=
				 factory->create_button("Insert column");

			 fields.remove_column=
				 factory->create_button("Remove column");

			 fields.insert_row=
				 factory->create_button("Insert row");

			 fields.remove_row=
				 factory->create_button("Remove row");
		 },
		 x::w::new_gridlayoutmanager());

	// Ok, we have all the fields, we can now construct the application
	// data object and attached it to the main_window.

	auto my_appdata=appdata::create(fields);
	main_window->appdata=my_appdata;

	// Excluding references in automatic scope, the main_window
	// owns a reference on the appdata, which owns references to all the
	// buttons and containers (and the main_window also has its own
	// references to its containers as child elements).
	//
	// The callbacks are owned by their respective display elements,
	// so the callbacks cannot capture references directly to the
	// main_window, or appdata. This will create a circular reference.
	//
	// To do this correctly, we'll use make_weak_capture() to capture
	// a weak reference to the main_window, with the callback recovering
	// a strong reference during execution (or doing nothing if the
	// weakly reference object no longer exists).

	my_appdata->insert_column->on_activate
		([main_window=x::make_weak_capture(main_window), counter=0]
		 (ONLY IN_THREAD,
		  const x::w::callback_trigger_t &trigger, const auto &busy)
		 mutable
		 {
			 auto got=main_window.get();

			 if (got)
			 {
				 auto& [main_window]= *got;

				 insert_column(main_window->appdata, ++counter);
			 }
		 });

	my_appdata->remove_column->on_activate
		([main_window=x::make_weak_capture(main_window)]
		 (ONLY IN_THREAD,
		  const x::w::callback_trigger_t &trigger, const auto &busy)
		 {
			 auto got=main_window.get();

			 if (got)
			 {
				 auto& [main_window]= *got;

				 remove_column(main_window->appdata);
			 }
		 });

	my_appdata->insert_row->on_activate
		([main_window=x::make_weak_capture(main_window), counter=0]
		 (ONLY IN_THREAD,
		  const x::w::callback_trigger_t &trigger, const auto &busy)
		 mutable
		 {
			 auto got=main_window.get();

			 if (got)
			 {
				 auto& [main_window]= *got;

				 insert_row(main_window->appdata, ++counter);
			 }
		 });

	my_appdata->remove_row->on_activate
		([main_window=x::make_weak_capture(main_window), counter=0]
		 (ONLY IN_THREAD,
		  const x::w::callback_trigger_t &trigger, const auto &busy)
		 mutable
		 {
			 auto got=main_window.get();

			 if (got)
			 {
				 auto& [main_window]= *got;

				 remove_row(main_window->appdata);
			 }
		 });

	update_button_state(my_appdata); // Initial button state.
}

// The grid layout manager demo.

void gridlayoutmanager()
{
	x::destroy_callback::base::guard guard;

	auto main_window=x::w::main_window::create(create_mainwindow);

	appdata my_appdata=main_window->appdata;

	main_window->on_disconnect([]
				   {
					   _exit(1);
				   });

	guard(main_window->connection_mcguffin());

	main_window->set_window_title("Grid Layout Manager");

	auto close_flag=close_flag_ref::create();

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
		gridlayoutmanager();
	} catch (const x::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
