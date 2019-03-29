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
#include <x/w/borderlayoutmanager.H>
#include <x/w/frame_appearance.H>
#include <x/w/label.H>

#include <string>
#include <iostream>
#include <sstream>
#include <utility>

#include "close_flag.H"

static void create_main_window(const x::w::main_window &mw)
{
	x::w::gridlayoutmanager glm=mw->get_layoutmanager();

	// Two sample containers with the border layout manager. One of them
	// will have a title.
	//
	// Even though the rest of the two containers are identical, the
	// title results in that one being slightly taller. For better
	// visual appearance, align all elements in row 0 at the bottom.
	glm->row_alignment(0, x::w::valign::bottom);

	x::w::gridfactory f=glm->append_row();

	x::w::new_borderlayoutmanager nblm
		{[&]
		 (const x::w::factory &f)
		 {
			 f->create_label("Border layout manager");
		 }};

	// We're just creating a simple label inside each border. Usually
	// an entire container gets created inside the border, but we just
	// have a label. Change the default padding between the border and
	// its element to 10 millimeters. This makes the container bigger,
	// for demo purposes.
	nblm.appearance=nblm.appearance->modify
		([]
		 (const auto &appearance)
		 {
			 appearance->hpad=10;
			 appearance->vpad=10;
		 });

	f->create_container([](const auto &){}, nblm);

	// Create the 2nd one with the title. We can use the same
	// new_borderlayoutmanager, just set the title.

	nblm.title("Hello");

	f->create_container([](const auto &){}, nblm);
}

void borderlayoutmanager()
{
	x::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto main_window=x::w::main_window::create(create_main_window);

	main_window->set_window_title("Borders!");
	main_window->set_window_class("main",
				      "borderlayoutmanager@examples.w.libcxx.com");

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

	main_window->show_all();

	x::mpcobj<bool>::lock lock{close_flag->flag};

	// update_title() updates the existing border's title.

	const char *titles[2]={"Hello", "World"};

	while (!lock.wait_for(std::chrono::seconds(1), [&] { return *lock; }))
	{
		std::swap(titles[0], titles[1]);

		// We want the container in row 0, column 1 of the main window's
		// grid.

		x::w::gridlayoutmanager glm=main_window->get_layoutmanager();

		x::w::container frame_with_border=glm->get(0, 1);

		// This container uses the border layout manager.
		x::w::borderlayoutmanager blm=
			frame_with_border->get_layoutmanager();

		// And update the border's title.
		blm->update_title(titles[0]);
	}
}

int main(int argc, char **argv)
{
	try {
		borderlayoutmanager();
	} catch (const x::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
