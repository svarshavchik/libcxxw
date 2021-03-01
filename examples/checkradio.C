/*
** Copyright 2017-2021 Double Precision, Inc.
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
#include <x/weakcapture.H>

#include <vector>
#include <tuple>
#include <string>

typedef std::vector<std::tuple<std::string, x::w::image_button>> buttons_t;

// The radio button for "Train" has a different label depending on whether
// the "Train" radio button is selected, or not.
//
// This is called from the radio button's callback, to update the radio
// button's label, and from create_radio(), to set the radio button's
// initial label. Both instances have a factory that's used to create the
// actual label, so this works out nicely.

void set_train_label(const x::w::factory &f, bool is_selected)
{
	f->create_label(is_selected ? "Train (no weekends)":"Train")

		// Once everything gets created, it's show_all()ed. But when
		// this is called to update the radio button's label, it is
		// our responsibility to show() the new display element.
		->show();
}

// Factored out for readability. This is the main part of the callback for
// the "Train" radio button, that's installed below.

void train_radio_callback(const x::w::image_button &train,

			  // Whether the radio button is now selected.
			  bool selected,

			  // The two checkboxes:

			  const x::w::image_button &saturday,
			  const x::w::image_button &sunday)
{
	std::cout << "Train: "
		  << (selected ? "":"not ")
		  << "checked"
		  << std::endl;

	// ... enable and disable the sunday and
	// saturday checkboxes.
	//

	sunday->set_enabled(!selected);
	saturday->set_enabled(!selected);

	// And update the label for the "Train"
	// radio button.

	train->update_label([selected]
			    (const x::w::factory &f)
			    {
				    set_train_label(f, selected);
			    });
}

// This is the creator lambda, that gets passed to create_mainwindow() below,
// factored out for readability.

void create_mainwindow(const x::w::main_window &main_window,
		       buttons_t &buttons)
{
	auto layout=main_window->gridlayout();

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
			// Add some padding to provide some separation from the
			// second set of columns with radio buttons, that
			// we'll add below.
			factory->right_padding(3)
			.create_checkbox([&]
					 (const x::w::factory &label_factory)
					 {
						 label_factory->create_label(day_of_week);
					 });


		// Set Monday through Friday checkboxes to value #2,
		// "indeterminate" state.
		if (n > 1 && n < 7)
			checkbox->set_value(2);

		// Install a callback that gets invoked whenever the checkbox
		// changes state.
		checkbox->on_activate([day_of_week]
				      (ONLY IN_THREAD,
				       size_t flag,
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

		buttons.push_back({day_of_week, checkbox});
	}

	// Append more columns to row #0.
	auto factory=layout->append_columns(0);

	// Create the "Train" button, initially set.

	x::w::image_button train=
		factory->create_radio("checkradiogroup@examples.w.libcxx.com",
				      []
				      (const x::w::factory &f)
				      {
					      // And the "set" label.
					      set_train_label(f, true);
				      });

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
	//
	// Note that this callback captures
	// references to the saturday and sunday display elements.
	//
	// Since all of these display elements are
	// in the same main window and neither one
	// is the parent of the other, when the
	// main window gets destroyed, it drops its
	// references on all these elements,
	// including the "train" checkbox, whose
	// callback has these captures. The
	// "train" element, and its callback, get
	// destroyed, destroying the captured
	// references too, allowing these display
	// elements to be destroyed as well.
	//
	// However, the callback also needs to capture a reference to its own
	// display element, and this obviously needs to be a weak reference.

	train->on_activate([saturday, sunday,
			    train=x::make_weak_capture(train)]
			   (ONLY IN_THREAD,
			    size_t flag,
			    const x::w::callback_trigger_t &trigger,
			    const auto &ignore2)
			   {
				   // Ignore this for the initial callback.

				   if (trigger.index() ==
				       x::w::callback_trigger_initial)
					   return;

				   // Recover the weak reference.

				   auto got_ref=train.get();

				   if (!got_ref)
					   return;

				   auto & [train]= *got_ref;

				   // Now, run the main logic of this callback.
				   train_radio_callback(train, flag > 0,
							saturday, sunday);
			   });

	// Append more columns to row #1.

	factory=layout->append_columns(1);

	// Create a "bus" radio button and label.
	x::w::image_button bus=
		factory->create_radio("checkradiogroup@examples.w.libcxx.com",
				      []
				      (const x::w::factory &f)
				      {
					      f->create_label("Bus");
				      });

	bus->on_activate([]
			 (ONLY IN_THREAD,
			  size_t flag,
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

	// Append more column to row #2

	factory=layout->append_columns(2);
	x::w::image_button drive=
		factory->create_radio("checkradiogroup@examples.w.libcxx.com",
				      []
				      (const x::w::factory &f)
				      {
					      f->create_label("Drive");
				      });

	drive->on_activate([]
			   (ONLY IN_THREAD,
			    size_t flag,
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

	// We create two more columns in rows 0 through 2. Grids should have
	// the same number of columns in each row, so what we'll do is use
	// create_canvas() to create an empty display element in rows 3 through
	// 6. Use colspan() to have the canvas span the two extra columns
	// that are taken up by the radio buttons.

	for (size_t i=3; i<7; ++i)
		layout->append_columns(i)->create_canvas();

	factory=layout->append_row();

	// A single element in the last row, spanning both columns, and
	// with some extra spacing above it.

	factory->colspan(2).top_padding(4);

	auto bottom_label=factory->create_label("Click here to take the train");

	// A separate display element that's considered to be an independent
	// "label" for a checkbox or a radio button. Clicking on this label
	// is equivalent to clicking on the "Train" radio button.
	//
	// The parameter to label_for must be a focusable display element.

	bottom_label->label_for(train);
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
	main_window->set_window_class("main",
				      "checkradio@examples.w.libcxx.com");

	main_window->on_delete
		([close_flag]
		 (ONLY IN_THREAD,
		  const x::w::busy &ignore)
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
