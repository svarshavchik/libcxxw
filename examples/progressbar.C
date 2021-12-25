/*
** Copyright 2017-2021 Double Precision, Inc.
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
#include <x/w/label.H>
#include <x/w/gridlayoutmanager.H>
#include <x/w/gridfactory.H>
#include <x/w/progressbar.H>

#include "close_flag.H"

#include <string>
#include <iostream>

std::string x::appid() noexcept
{
	return "progressbar.examples.w.libcxx.com";
}

// Create the progress bar.

static auto initialize_progressbar(const x::w::factory &f)
{
	// progressbar_config is an optional parameter to create_progressbar().
	//
	// It provides the progress bar's initial value and maximum value,
	// as well as a custom appearance object.

	x::w::progressbar_config config{0, 100};

	auto pb=f->create_progressbar
		([]
		 (const x::w::progressbar &pb)
		 {
			 // We are creating the progress bar with the
			 // grid layout manager. Put one element into the
			 // progress bar, our label.

			 auto glm=pb->gridlayout();

			 auto f=glm->append_row();

			 // Position the label centered, in the progress bar.

			 f->halign(x::w::halign::center);
			 f->create_label("0%")->show();
		 },

		 // Optional parameter: initial configuration
		 config,

		 // Optional parameter: the grid layout manager, by default.

		 x::w::new_gridlayoutmanager{});

	pb->show();

	return pb;
}

void showprogressbar()
{
	x::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto main_window=x::w::main_window
		::create([]
			 (const auto &main_window)
			 {
				 auto layout=main_window->gridlayout();
				 x::w::gridfactory factory=
					 layout->append_row();

				 initialize_progressbar(factory);
			 });

	// Retrieve the progress bar, and the label elements from the newly-
	// created main_window.

	// Both the main window and the progress bar containers are grid
	// layout managers, with one display element: row 0, column 0.

	x::w::progressbar pb=main_window->gridlayout()->get(0, 0);

	x::w::label l=pb->gridlayout()->get(0, 0);

	main_window->set_window_title("Progress bar!");
	main_window->set_window_class("main",
				      "progressbar.examples.w.libcxx.com");

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

	// Count the progress bar, and go away.

	int v=0;

	while (1)
	{
		lock.wait_for(std::chrono::milliseconds(500),
			      [&] { return *lock; });
		if (*lock) break;

		if (v >= 100)
			break;
		v += 5;

		// Update the progress bar to reflect the completion
		// percentage.
		//
		// It's up to us to update the label inside the progress bar.
		// We format "x%" ourselves, and update the label.
		//
		// Progress bar's update() method updates the progress bar's
		// visual slider. The third parameter to update() is an
		// optional closure. The closure parameter is an optimization
		// technique.
		//
		// Updating the label redraws it, and updating the
		// progress bar ends up redrawing the label again, to include
		// the slider that "slides" under the label. Passing a closure
		// to update() executes the closure and updates the progress
		// bar in one operation, redrawing the whole thing in one shot.

		std::ostringstream o;

		o << v << '%';

		pb->update(v, 100, [txt=o.str(), l]
			   (ONLY IN_THREAD)
			   {
				   l->update(txt);
			   });
	}
}

int main(int argc, char **argv)
{
	try {
		showprogressbar();
	} catch (const x::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
