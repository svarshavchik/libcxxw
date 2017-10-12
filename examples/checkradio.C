/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "config.h"
#include "close_flag.H"

#include <x/exception.H>
#include <x/destroy_callback.H>

#include <x/w/main_window.H>
#include <x/w/gridlayoutmanager.H>
#include <x/w/gridfactory.H>
#include <x/w/label.H>
#include <x/w/canvas.H>
#include <x/w/container.H>
#include <x/w/image_button.H>
#include <x/w/radio_group.H>

#include <vector>
#include <tuple>
#include <string>

typedef std::vector<std::tuple<std::string, x::w::image_button>> buttons_t;

// This is the creator lambda, that gets passed to create_mainwindow() below,
// factored out for readability.

void create_mainwindow(const x::w::main_window &main_window,
		       buttons_t &buttons)
{
	x::w::gridlayoutmanager layout=main_window->get_layoutmanager();

	static const char * const days_of_week[]={
		"Sunday",
		"Monday",
		"Tuesday",
		"Wednesday",
		"Thursday",
		"Friday",
		"Saturday"};

	std::vector<x::w::image_button> days_of_week_checkboxes;

	int n=0;

	// In the first column of each row put a checkbox.
	// Create a label in the second column of each row, with the name
	// of the day of the week.

	for (auto day_of_week:days_of_week)
	{
		++n;

		x::w::gridfactory factory=layout->append_row();

		x::w::image_button checkbox=
			factory->valign(x::w::valign::middle).create_checkbox();

		// Set Monday through Friday checkboxes to value #2,
		// "indeterminate" state.
		if (n > 1 && n < 7)
			checkbox->set_value(2);

		// Install a callback that gets invoked whenever the checkbox
		// changes state.
		checkbox->on_activate([day_of_week]
				      (size_t flag,
				       const x::w::callback_trigger_t &trigger,
				       const auto &ignore)
				      {
					      if (trigger.index() ==
						  x::w::callback_trigger_initial)
						      return;

					      std::cout << day_of_week
							<< ": "
							<< (flag ? "":"not ")
							<< "checked"
							<< std::endl;
				      });
		days_of_week_checkboxes.push_back(checkbox);

		// Add some padding to provide some separation from the
		// second set of columns with radio buttons, that we'll add
		// below.

		auto label=factory->right_padding(3)
			.create_label({day_of_week});

		label->label_for(checkbox);

		buttons.push_back({day_of_week, checkbox});
	}

	// Create a radio button group, to tie all the radio buttons together.
	x::w::radio_group group=x::w::radio_group::create();

	// Append more columns to row #0.
	auto factory=layout->append_columns(0);

	// Create a radio button
	x::w::image_button train=factory->valign(x::w::valign::middle)
		.create_radio(group);

	// Set this radio button.
	train->set_value(1);

	// Whenever this checkbox is enabled, the "sunday" and "saturday"
	// checkboxes are disabled. Since the initial state of this radio
	// button is the set state, we'll disable them right off the bat.

	auto sunday=*days_of_week_checkboxes.begin();
	auto saturday=*--days_of_week_checkboxes.end();

	sunday->set_enabled(false);
	saturday->set_enabled(false);

	// Install an on_activate() callback for the "Train" radio button,
	// which will ...

	train->on_activate([saturday, sunday]
			   (size_t flag,
			    const x::w::callback_trigger_t &trigger,
			    const auto &ignore2)
			   {
				   if (trigger.index() ==
				       x::w::callback_trigger_initial)
					   return;

				   std::cout << "Train: "
					     << (flag ? "":"not ")
					     << "checked"
					     << std::endl;

				   // ... enable and disable the sunday and
				   // saturday checkboxes.
				   //
				   // Note that this callback captures
				   // references to these display elements.
				   //
				   // Since all of these display elements are
				   // in the same main window and neither one
				   // is the parent of the other, when the
				   // main window gets destroyed, it drops its
				   // references on all these elements,
				   // including the "train" checkbox, whose
				   // callback has these two captures. The
				   // "train" element, and its callback, get
				   // destroyed, destroying the captured
				   // references too, allowing these display
				   // elements to be destroyed as well.

				   sunday->set_enabled(!flag);
				   saturday->set_enabled(!flag);
			   });

	// Label this radio button.
	factory->create_label({"Train"})->label_for(train);

	// Append more columns to row #1.

	factory=layout->append_columns(1);

	// Create a "bus" radio button and label.
	x::w::image_button bus=factory->valign(x::w::valign::middle)
		.create_radio(group);
	bus->on_activate([]
			 (size_t flag,
			  const x::w::callback_trigger_t &trigger,
			  const auto &ignore)
			 {
				 if (trigger.index() ==
				     x::w::callback_trigger_initial)
					 return;

				 std::cout << "Bus: "
					   << (flag ? "":"not ")
					   << "checked"
					   << std::endl;
			 });

	factory->create_label({"Bus"})->label_for(bus);

	// Append more column to row #2

	factory=layout->append_columns(2);
	x::w::image_button drive=factory->valign(x::w::valign::middle)
		.create_radio(group);
	drive->on_activate([]
			   (size_t flag,
			    const x::w::callback_trigger_t &trigger,
			    const auto &ignore)
			   {
				   if (trigger.index() ==
				       x::w::callback_trigger_initial)
					   return;

				   std::cout << "Drive: "
					     << (flag ? "":"not ")
					     << "checked"
					     << std::endl;
			   });
	factory->create_label({"Drive"})->label_for(drive);

	// We create two more columns in rows 0 through 2. Grids should have
	// the same number of columns in each row, so what we'll do is use
	// create_canvas() to create an empty display element in rows 3 through
	// 6. Use colspan() to have the canvas span the two extra columns
	// that are taken up by the radio buttons.

	for (size_t i=3; i<7; ++i)
		layout->append_columns(i)->colspan(2)
			.create_canvas();
}

void checkradio()
{
	x::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	buttons_t buttons;

	auto main_window=x::w::main_window::create
		([&]
		 (const auto &main_window)
		 {
			 create_mainwindow(main_window, buttons);
		 });

	main_window->on_disconnect([]
				   {
					   _exit(1);
				   });

	guard(main_window->connection_mcguffin());

	main_window->set_window_title("Checkboxes");

	main_window->on_delete
		([close_flag]
		 (const x::w::busy &ignore)
		 {
			 close_flag->close();
		 });

	main_window->show_all();

	close_flag->wait();

	// Read the final values of the days of week checkbox.

	for (const auto &checkbox:buttons)
		std::cout << std::get<std::string>(checkbox)
			  << ": "
			  << std::get<x::w::image_button>(checkbox)->get_value()
			  << std::endl;
}

int main(int argc, char **argv)
{
	try {
		checkradio();
	} catch (const x::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
