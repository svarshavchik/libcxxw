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
#include <x/w/screen.H>
#include <x/w/connection.H>
#include <string>
#include <iostream>

#include "close_flag.H"
#include "combobox.H"

static inline auto
create_standard_combobox(const x::w::factory &factory,
			 void (*creator)(const x::w::focusable_container &))
{
	x::w::new_standard_comboboxlayoutmanager
		nclm([]
		     (const x::w::standard_combobox_selection_changed_info_t &i)
		     {
			     if (i.selected_flag)
			     {
				     std::cout << "Selected item #"
					       << i.item_index
					       << std::endl;
			     }
		     });

	return factory->create_focusable_container(creator, nclm);
}

static inline auto
create_editable_combobox(const x::w::factory &factory,
			 void (*creator)(const x::w::focusable_container &))
{
	x::w::new_editable_comboboxlayoutmanager
		nclm([]
		     (const x::w::editable_combobox_selection_changed_info_t &i)
		     {
			     if (i.selected_flag)
			     {
				     std::cout << "Selected item #"
					       << i.item_index
					       << std::endl;
			     }
		     });

	return factory->create_focusable_container(creator, nclm);
}

static inline void create_main_window(const x::w::main_window &main_window,
				      const options &opts)
{
	x::w::gridlayoutmanager layout=main_window->get_layoutmanager();

	layout->row_alignment(0, x::w::valign::middle);

	auto factory=layout->append_row();

	factory->create_label("Choose a day of the week:")->show();

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

			 lm->replace_all({"Sunday",
					  "Monday",
					  "Tuesday",
					  "Wednesday",
					  "Thursday",
					  "Friday",
					  "Saturday"});
		 });

	combobox->show();

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

	guard(main_window->connection_mcguffin());

	main_window->on_disconnect([]
				   {
					   _exit(1);
				   });

	main_window->on_delete
		([close_flag]
		 {
			 close_flag->close();
		 });

	main_window->show();

	x::mpcobj<bool>::lock lock{close_flag->flag};

	lock.wait([&] { return *lock; });

	x::w::focusable_container combobox=main_window->appdata;

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
