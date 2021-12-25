/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "config.h"
#include <x/mpobj.H>
#include <x/appid.H>
#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/pidinfo.H>
#include <x/w/main_window.H>
#include <x/w/gridlayoutmanager.H>
#include <x/w/standard_comboboxlayoutmanager.H>
#include <x/w/editable_comboboxlayoutmanager.H>
#include <x/w/input_field_lock.H>
#include <x/w/focusable_container.H>
#include <x/w/gridfactory.H>
#include <x/w/label.H>
#include <x/w/button.H>
#include <x/w/uielements.H>
#include <x/w/uigenerators.H>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>

#include "close_flag.H"

std::string x::appid() noexcept
{
	return "uigenerator1.examples.w.libcxx.com";
}

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


std::vector<x::w::list_item_param> days_of_week()
{
	return {"Sunday", "Monday", "Tuesday",	"Wednesday",
			"Thursday", "Friday", "Saturday"};
}

static inline void create_main_window(const x::w::main_window &main_window)
{
	std::string me=x::exename(); // My path.
	size_t p=me.rfind('/');

	// Load "uigenerator1.xml" from the same directory as me

	x::w::const_uigenerators generator=
		x::w::const_uigenerators::create(me.substr(0, ++p) +
						 "uigenerator1.xml");

	x::w::containerptr combobox;
	x::w::buttonptr append_row_button;
	x::w::buttonptr insert_row_button;
	x::w::buttonptr replace_row_button;
	x::w::buttonptr delete_row_button;
	x::w::buttonptr reset_button;
	x::w::buttonptr shuffle_button;

	// The first member in the element factory is a map with callbacks,
	// identified by a text label, that get referenced from the theme
	// file. The first one, "combobox-label" gets referenced by the
	// <name>d <element> in the theme file. The <element> instruction
	// invokes the callback, and passes the current factory. The callback
	// creates a label widget.
	x::w::uielements element_factory
		{
		 {
		  {"combobox-label",
		   []
		   (const x::w::factory &factory)
		   {
			   factory->create_label("Days of the week (or else):");
		   }
		  },

		  {"combobox-element",
		   [&]
		   (const x::w::factory &factory)
		   {
			   combobox=create_standard_combobox
				   (factory,
				    []
				    (const auto &container)
				    {
					    auto lm=container
						    ->standard_comboboxlayout();

					    lm->replace_all_items
						    (days_of_week());
				    });
		   },
		  },
		  {"append-row-button",
		   [&]
		   (const x::w::factory &factory)
		   {
			   append_row_button=
				   factory->create_button("Append row");
		   },
		  },
		  {"insert-row-button",
		   [&]
		   (const x::w::factory &factory)
		   {
			   insert_row_button=
				   factory->create_button("Insert row");
		   },
		  },
		  {"replace-row-button",
		   [&]
		   (const x::w::factory &factory)
		   {
			   replace_row_button=
				   factory->create_button("Replace row");
		   },
		  },
		  {"delete-row-button",
		   [&]
		   (const x::w::factory &factory)
		   {
			   delete_row_button=
				   factory->create_button("Delete row");
		   },
		  },
		  {"reset-button",
		   [&]
		   (const x::w::factory &factory)
		   {
			   reset_button=
				   factory->create_button("Reset");
		   },
		  },
		  {"shuffle-button",
		   [&]
		   (const x::w::factory &factory)
		   {
			   shuffle_button=
				   factory->create_button("Shuffle");
		   },
		  },
		 }
		};

	auto layout=main_window->gridlayout();

	layout->generate("main-window-grid",
			 generator, element_factory);

	append_row_button->on_activate
		([=, counter=0]
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

	insert_row_button->on_activate
		([=, counter=0]
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

	replace_row_button->on_activate
		([=, counter=0]
		 (ONLY IN_THREAD,
		  const x::w::callback_trigger_t &trigger,
		  const x::w::busy &ignore)
		 mutable
		 {
			 x::w::standard_comboboxlayoutmanager lm=
				 combobox->get_layoutmanager();

			 x::w::standard_combobox_lock lock{lm};

			 if (lm->size() == 0)
				 return;

			 std::ostringstream o;

			 o << "Replace " << ++counter << std::endl;

			 lm->replace_items(IN_THREAD, 0, {o.str()});
		 });

	delete_row_button->on_activate
		([combobox]
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

	reset_button->on_activate
		([combobox]
		 (ONLY IN_THREAD,
		  const x::w::callback_trigger_t &trigger,
		  const x::w::busy &ignore)
		 {
			 x::w::standard_comboboxlayoutmanager lm=
				 combobox->get_layoutmanager();

			 lm->replace_all_items(IN_THREAD,
					       days_of_week());
		 });

	shuffle_button->on_activate
		([combobox]
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

	main_window->appdata=combobox;
}

void uigenerator1()
{
	x::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto main_window=x::w::main_window
		::create([&]
			 (const auto &main_window)
			 {
				 create_main_window(main_window);
			 });

	main_window->set_window_class("main",
				      "uigenerator1.examples.w.libcxx.com");

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

	main_window->show_all();

	close_flag->wait();

	x::w::focusable_container combobox=main_window->appdata;

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
		uigenerator1();
	} catch (const x::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
