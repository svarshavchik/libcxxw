/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "config.h"
#include <x/mpobj.H>
#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/w/main_window.H>
#include <x/w/gridlayoutmanager.H>
#include <x/w/standard_comboboxlayoutmanager.H>
#include <x/w/editable_comboboxlayoutmanager.H>
#include <x/w/input_field_lock.H>
#include <x/w/focusable_container.H>
#include <x/w/gridfactory.H>
#include <x/w/label.H>
#include <x/w/button.H>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>

#include "close_flag.H"
#include "combobox.H"

static inline auto
create_standard_combobox(const x::w::factory &factory,
			 void (*creator)(const x::w::focusable_container &))
{

	x::w::new_standard_comboboxlayoutmanager nclm{

		// Default callback that gets invoked whenever any combobox
		// list item gets selected or unselected.

		[]
		(ONLY IN_THREAD,
		 const x::w::standard_combobox_selection_changed_info_t &i)
		{
			if (!i.list_item_status_info.selected)
				return;

			std::cout << "Selected item #"
				  << i.list_item_status_info.item_number
				  << std::endl;
		}};

	return factory->create_focusable_container(creator, nclm);
}

static inline auto
create_editable_combobox(const x::w::factory &factory,
			 void (*creator)(const x::w::focusable_container &))
{
	x::w::new_editable_comboboxlayoutmanager nclm{

		// Default callback that gets invoked whenever any combobox
		// list item gets selected or unselected.

		[]
		(ONLY IN_THREAD,
		 const x::w::editable_combobox_selection_changed_info_t &i)
		{
			if (!i.list_item_status_info.selected)
				return;

			std::cout << "Selected item #"
				  << i.list_item_status_info.item_number
				  << std::endl;
		}};

	return factory->create_focusable_container(creator, nclm);
}

// append_items(), insert_items(), et. al., take a vector of list_items_params
// as a parameter. We can simply prepare the list of items in advance, like
// this, and pass it.

std::vector<x::w::list_item_param> days_of_week()
{
	return {"Sunday", "Monday", "Tuesday",	"Wednesday",
			"Thursday", "Friday", "Saturday"};
}

static inline void create_main_window(const x::w::main_window &main_window,
				      const options &opts)
{
	x::w::gridlayoutmanager layout=main_window->get_layoutmanager();

	layout->row_alignment(0, x::w::valign::middle);

	auto factory=layout->append_row();

	factory->create_label("Days of the week (or else):")->show();

	// Note that whether it's an editable or the standard combo-box,
	// it's still the same focusable container object. The only
	// immediate difference is the layout manager.

	auto combobox=(opts.editable->value
		       ? create_editable_combobox
		       : create_standard_combobox)
		(factory,
		[]
		 (const auto &container)
		 {
			 // An editable_comboboxlayoutmanager is a subclass of
			 // standard_comboboxlayoutmanager, so with either the
			 // standard or the editable combo-box we can use a
			 // standard_comboboxlayoutmanager to initialize it.

			 x::w::standard_comboboxlayoutmanager lm=
				 container->get_layoutmanager();

			 lm->replace_all_items(days_of_week());
		 });

	combobox->show();

	// Combo-box layout manager inherits methods from the underlying
	// list layout manager, like append_items().
	//
	// Create buttons on the rows below, with each button demonstrating
	// each combo-box operation. The combo-box layout manager inherits
	// append_items(), insert_items(), et. al., from the underlying
	// list layout manager.
	//
	// The buttons' callbacks get invoked by the connection thread and
	// receive the IN_THREAD handle; as such the callbacks can use
	// the append_items(), insert_items(), et. al., overloads that
	// take an IN_THREAD parameter.
	//
	// Like a selection list, the combo-box's contents get updated
	// by the connection thread. Calling the IN_THREAD overloads reflects
	// the changes in the combo-box's contents immediately. A main
	// execution thread invoking the non-IN_THREAD overloads results in
	// a message getting set to the connection thread, and the overloaded
	// methods returning immediately. This means that, for example,
	// calling insert_items() or append_items() from a non-IN_THREAD
	// contents, followed by size(), will likely report the combo-box's
	// size apparently unchanged. This is because the new items will
	// actually get inserted by the connection thread when it processes
	// the message. Caveat emptor.

	auto button=layout->append_row()->colspan(2)
		.create_button("Append row");

	button->on_activate([=, counter=0]
			    (ONLY IN_THREAD,
			     const x::w::callback_trigger_t &trigger,
			     const x::w::busy &ignore)
			    mutable
			    {
				    std::ostringstream o;

				    o << "Append " << ++counter;

				    x::w::standard_comboboxlayoutmanager lm=
					    combobox->get_layoutmanager();

				    lm->append_items(IN_THREAD, {o.str()});
			    });
	button->show();

	// Same deal for insert_items().

	button=layout->append_row()->colspan(2)
		.create_button("Insert row");

	button->on_activate([=, counter=0]
			    (ONLY IN_THREAD,
			     const x::w::callback_trigger_t &trigger,
			     const x::w::busy &ignore)
			    mutable
			    {
				    std::ostringstream o;

				    o << "Insert " << ++counter << std::endl;

				    x::w::standard_comboboxlayoutmanager lm=
					    combobox->get_layoutmanager();

				    lm->insert_items(IN_THREAD, 0, {o.str()});
			    });
	button->show();

	// Same deal for replace_items(), delete_item(), size(), and
	// replace_all_items().

	button=layout->append_row()->colspan(2)
		.create_button("Replace row");

	button->on_activate([=, counter=0]
			    (ONLY IN_THREAD,
			     const x::w::callback_trigger_t &trigger,
			     const x::w::busy &ignore)
			    mutable
			    {
				    x::w::standard_comboboxlayoutmanager lm=
					    combobox->get_layoutmanager();

				    // Instantiating a standard_combo_box
				    // blocks all other execution threads from
				    // accessing the contents of the combo-box.
				    //
				    // After we check size(), below, it is
				    // theoretically possible for another thread
				    // to modify it.
				    //
				    // This is not really needed here, this
				    // is for demonstration purposes only.
				    // This example modifies the contents of
				    // the combo-box only IN_THREAD, so this
				    // doesn't really do anything.

				    x::w::standard_combobox_lock
					    lock{lm};

				    if (lm->size() == 0)
					    return;

				    std::ostringstream o;

				    o << "Replace " << ++counter << std::endl;

				    // For convenience, all modification
				    // functions are also implemented by
				    // standard_combobox_lock. The following
				    // is equivalent to:
				    //
				    // lock.replace_items( ... )

				    lm->replace_items(IN_THREAD, 0, {o.str()});
			    });
	button->show();

	button=layout->append_row()->colspan(2)
		.create_button("Delete row");

	button->on_activate([combobox]
			    (ONLY IN_THREAD,
			     const x::w::callback_trigger_t &trigger,
			     const x::w::busy &ignore)
			    {
				    x::w::standard_comboboxlayoutmanager lm=
					    combobox->get_layoutmanager();

				    if (lm->size() == 0)
					    return;

				    lm->remove_item(IN_THREAD, 0);
			    });
	button->show();

	button=layout->append_row()->colspan(2)
		.create_button("Reset");

	button->on_activate([combobox]
			    (ONLY IN_THREAD,
			     const x::w::callback_trigger_t &trigger,
			     const x::w::busy &ignore)
			    {
				    x::w::standard_comboboxlayoutmanager lm=
					    combobox->get_layoutmanager();

				    lm->replace_all_items(IN_THREAD,
							  days_of_week());
			    });
	button->show();

	// Use resort_items() to randomly shuffle the list items.

	button=layout->append_row()->colspan(2)
		.create_button("Shuffle");

	button->on_activate([combobox]
			    (ONLY IN_THREAD,
			     const x::w::callback_trigger_t &trigger,
			     const x::w::busy &ignore)
			    {
				    x::w::standard_comboboxlayoutmanager lm=
					    combobox->get_layoutmanager();

				    std::vector<size_t> n;

				    n.resize(lm->size());

				    std::generate(n.begin(), n.end(),
						  [n=0]
						  ()
						  mutable
						  {
							  return n++;
						  });

				    std::random_shuffle(n.begin(), n.end());

				    lm->resort_items(IN_THREAD, n);
			    });
	button->show();
	// Stash the combobox in main_window's appdata.

	main_window->appdata=combobox;
}

void create_combobox(const options &opts)
{
	x::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto main_window=x::w::main_window
		::create([&]
			 (const auto &main_window)
			 {
				 create_main_window(main_window, opts);
			 });

	main_window->set_window_title(opts.editable->value ?
				      "Editable combo-box"
				      : "Standard combo-box");
	main_window->set_window_class("main",
				      "combobox@examples.w.libcxx.com");

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

	main_window->show();

	close_flag->wait();

	x::w::focusable_container combobox=main_window->appdata;

	// For an editable combobox, the layout manager can be used to
	// construct an input_lock, providing access to the contents of the
	// input field.

	if (opts.editable->value)
	{
		x::w::editable_comboboxlayoutmanager lm=
			combobox->get_layoutmanager();
		x::w::input_lock lock{lm};

		std::cout << "Final contents: " << lock.get() << std::endl;
	}

	x::w::standard_comboboxlayoutmanager lm=
		combobox->get_layoutmanager();

	std::optional<size_t> selection=lm->selected();

	if (selection)
	{
		std::cout << "Final selection was day #" << selection.value()
			  << std::endl;
	}
	else
	{
		std::cout << "No combo-box selection."
			  << std::endl;
	}
}

int main(int argc, char **argv)
{
	try {
		options opts;

		opts.parse(argc, argv);

		create_combobox(opts);
	} catch (const x::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
