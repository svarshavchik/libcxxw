/*
** Copyright 2017 Double Precision, Inc.
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
		(const x::w::standard_combobox_selection_changed_info_t &i)
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
		(const x::w::editable_combobox_selection_changed_info_t &i)
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
	// list layout manager, like append_items(). Create a button that
	// calls append_items().

	auto button=layout->append_row()->colspan(2)
		.create_special_button_with_label("Append row");

	button->on_activate([=, counter=0]
			    (const x::w::callback_trigger_t &trigger,
			     const x::w::busy &ignore)
			    mutable
			    {
				    std::ostringstream o;

				    o << "Append " << ++counter << std::endl;

				    x::w::standard_comboboxlayoutmanager lm=
					    combobox->get_layoutmanager();

				    lm->append_items({o.str()});
			    });
	button->show();

	// Same deal for insert_items().

	button=layout->append_row()->colspan(2)
		.create_special_button_with_label("Insert row");

	button->on_activate([=, counter=0]
			    (const x::w::callback_trigger_t &trigger,
			     const x::w::busy &ignore)
			    mutable
			    {
				    std::ostringstream o;

				    o << "Insert " << ++counter << std::endl;

				    x::w::standard_comboboxlayoutmanager lm=
					    combobox->get_layoutmanager();

				    lm->insert_items(0, {o.str()});
			    });
	button->show();

	// Same deal for replace_items(), delete_item(), and size():

	button=layout->append_row()->colspan(2)
		.create_special_button_with_label("Replace row");

	button->on_activate([=, counter=0]
			    (const x::w::callback_trigger_t &trigger,
			     const x::w::busy &ignore)
			    mutable
			    {
				    x::w::standard_comboboxlayoutmanager lm=
					    combobox->get_layoutmanager();

				    if (lm->size() == 0)
					    return;

				    std::ostringstream o;

				    o << "Replace " << ++counter << std::endl;

				    lm->replace_items(0, {o.str()});
			    });
	button->show();

	button=layout->append_row()->colspan(2)
		.create_special_button_with_label("Delete row");

	button->on_activate([=, counter=0]
			    (const x::w::callback_trigger_t &trigger,
			     const x::w::busy &ignore)
			    mutable
			    {
				    x::w::standard_comboboxlayoutmanager lm=
					    combobox->get_layoutmanager();

				    if (lm->size() == 0)
					    return;

				    lm->remove_item(0);
			    });
	button->show();

	button=layout->append_row()->colspan(2)
		.create_special_button_with_label("Reset");

	button->on_activate([=, counter=0]
			    (const x::w::callback_trigger_t &trigger,
			     const x::w::busy &ignore)
			    mutable
			    {
				    x::w::standard_comboboxlayoutmanager lm=
					    combobox->get_layoutmanager();

				    lm->replace_all_items(days_of_week());
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
		 (const x::w::busy &ignore)
		 {
			 close_flag->close();
		 });

	main_window->show();

	x::mpcobj<bool>::lock lock{close_flag->flag};

	lock.wait([&] { return *lock; });

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
