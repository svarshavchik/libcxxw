#include "libcxxw_config.h"
#include "creator/app.H"
#include "x/w/uielements.H"
#include "x/w/impl/uixmlparser.H"
#include "x/w/standard_comboboxlayoutmanager.H"
#include "x/w/editable_comboboxlayoutmanager.H"
#include "x/w/tablelayoutmanager.H"
#include "x/w/pagelayoutmanager.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/input_field_lock.H"
#include "x/w/input_field.H"
#include "x/w/image_button.H"
#include "x/w/label.H"
#include "x/w/button.H"
#include "x/w/uigenerators.H"
#include "x/w/scrollbar.H"
#include "x/w/text_param_literals.H"
#include "messages.H"
#include "catch_exceptions.H"
#include <x/messages.H>
#include <x/strtok.H>
#include <x/weakcapture.H>
#include <x/mpthreadlock.H>
#include <x/visitor.H>
#include <x/strtok.H>
#include <x/locale.H>

void appObj::appearances_elements_initialize(app_elements_tptr &elements,
					     x::w::uielements &ui,
					     init_args &args,
					     const x::w::main_window &mw,
					     const x::messages &catalog,
					     const x::w::screen_positions &pos)
{
	auto appearance_dialog_gen=
		x::w::const_uigenerators::create(CREATORDIR
						 "/appearance_dialog.xml", pos,
						 catalog);

	args.elements.appearance_dialog_gen=appearance_dialog_gen;

	// Create the appearance dialog

	x::w::create_dialog_args appearance_args
		{"appearance_dialog@cxxwcreator.w.libcxx.com",
		 true};

	appearance_args.restore(pos,
				"appearance_dialog");

	args.elements.appearance_dialog=mw->create_dialog
		(appearance_args,
		 [&](const auto &d)
		 {
			 x::w::gridlayoutmanager
				 glm=d->dialog_window->get_layoutmanager();

			 glm->generate("main",
				       appearance_dialog_gen,
				       ui);
		 });

	x::w::label appearance_new_value_description=
		ui.get_element("appearance_new_value_description");

	x::w::image_button appearance_new_value_option=
		ui.get_element("appearance_new_value_option");

	x::w::image_button appearance_reset_value_option=
		ui.get_element("appearance_reset_value_option");

	x::w::container appearance_new_value_container=
		ui.get_element("appearance_new_value_container");

	x::w::button appearance_new_value_save=
		ui.get_element("appearance_new_value_save");

	x::w::button appearance_new_value_cancel=
		ui.get_element("appearance_new_value_cancel");

	// The "new value" radio button's state has changed, call
	// appearance_value_option_selected.
	appearance_new_value_option->on_activate
		([&]
		 (ONLY IN_THREAD,
		  size_t n,
		  const auto &trigger,
		  const auto &mcguffin)
		 {
			 appinvoke(&appObj::appearance_value_option_selected,
				   IN_THREAD, n);
		 });

	// Closing the new appearance value dialog.
	appearance_new_value_cancel->on_activate
		([]
		 (ONLY IN_THREAD,
		  const auto &grigger,
		  const auto &busy)
		 {
			 appinvoke([&]
				   (appObj *me)
				   {
					   me->appearance_dialog->dialog_window
						   ->hide(IN_THREAD);
				   });
		 });

	elements.appearance_new_value_description=
		appearance_new_value_description;
	elements.appearance_new_value_option=
		appearance_new_value_option;
	elements.appearance_reset_value_option=
		appearance_reset_value_option;
	elements.appearance_new_value_save=appearance_new_value_save;
	elements.appearance_new_value_container=appearance_new_value_container;

	x::w::focusable_container appearance_name=
		ui.get_element("appearance_name_field");
	x::w::label appearance_new_name_label=
		ui.get_element("appearance_new_name_label");
	x::w::input_field appearance_new_name=
		ui.get_element("appearance_new_name_field");
	x::w::label appearance_new_name_label2=
		ui.get_element("appearance_new_name_label2");
	x::w::focusable_container appearance_new_type=
		ui.get_element("appearance_new_type_field");
	x::w::label appearance_new_name_label3=
		ui.get_element("appearance_new_name_label3");
	x::w::button appearance_new_create=
		ui.get_element("appearance_new_create");
	x::w::container appearance_contents_page=
		ui.get_element("appearance_contents_page");
	x::w::focusable_container appearance_based_on_field=
		ui.get_element("appearance_based_on_field");
	x::w::focusable_container appearance_values=
		ui.get_element("appearance_values");
	x::w::container appearance_values_popup=
		ui.get_element("appearance_values_popup");
	x::w::button appearance_update_button=
		ui.get_element("appearance_update_button");
	x::w::button appearance_delete_button=
		ui.get_element("appearance_delete_button");
	x::w::button appearance_reset_button=
		ui.get_element("appearance_reset_button");

	x::w::listitemhandle appearance_value_move_up=
		ui.get_listitemhandle("appearance_value_move_up");
	x::w::listitemhandle appearance_value_move_down=
		ui.get_listitemhandle("appearance_value_move_down");
	x::w::listitemhandle appearance_value_update=
		ui.get_listitemhandle("appearance_value_update");
	x::w::listitemhandle appearance_value_create=
		ui.get_listitemhandle("appearance_value_create");
	x::w::listitemhandle appearance_value_delete=
		ui.get_listitemhandle("appearance_value_delete");

	// Initialize the combo-box of a type for a new appearance.
	{
		auto appearance_type_lm=
			appearance_new_type->standard_comboboxlayout();

		std::vector<x::w::list_item_param> params;

		params.reserve(args.appearance_types.size());

		for (const auto &at:args.appearance_types)
			params.push_back(at.first);

		appearance_type_lm->replace_all_items(params);

		appearance_type_lm->on_selection_changed
			([]
			 (ONLY IN_THREAD,
			  const auto &info)
			 {
				 appinvoke(&appObj
					   ::new_appearance_type_selected,
					   IN_THREAD,
					   info);
			 });
	}

	// Name of a new appearance: allow only valid characters.
	appearance_new_name->on_filter(get_label_filter());

	// And enable/disable buttons. Must have a non-empty field to enable
	// the save button.
	appearance_new_name->on_change
		([]
		 (ONLY IN_THREAD,
		  const auto &change_info)
		 {
			 appinvoke
				 ([&]
				  (appObj *me)
				  {
					  appearance_info_t::lock lock
						  {me->appearance_info};

					  me->appearance_enable_disable_buttons
						  (IN_THREAD, lock);
				  });
		 });

	// A new appearance has been selected.
	//
	// Invoke appearance_reset() when the appearance name combo-box
	// changes value.
	auto appearance_name_lm=appearance_name->standard_comboboxlayout();

	appearance_name_lm->on_selection_changed
		([]
		 (ONLY IN_THREAD,
		  const auto &info)
		 {
			 if (!info.list_item_status_info.selected)
				 return;

			 appinvoke(&appObj::appearance_reset, IN_THREAD);
		 });

	// "Based on" combo-box.
	auto appearance_based_on_lm=
		appearance_based_on_field->standard_comboboxlayout();

	appearance_based_on_lm->on_selection_changed
		([]
		 (ONLY IN_THREAD,
		  const auto &info)
		 {
			 if (!info.list_item_status_info.selected)
				 return;

			 appinvoke([&]
				   (auto *me)
				   {
					   appearance_info_t::lock
						   lock{me->appearance_info};

					   me->appearance_enable_disable_buttons
						   (IN_THREAD, lock);
				   });
		 });

	auto appearance_values_lm=appearance_values->tablelayout();

	// Selecting an appearance value opens a popup
	appearance_values_lm->selection_type
		([]
		 (ONLY IN_THREAD,
		  const auto &lm,
		  size_t i,
		  const auto &trigger,
		  const auto &busy)
		 {
			 appinvoke
				 ([&]
				  (appObj *me)
				 {
					 lm->unselect(IN_THREAD);
					 lm->selected(IN_THREAD, i,
						      true, trigger);

					 // move up/down below autoselect()s
					 // the moved button.
					 //
					 // We don't want to reopen the popup,
					 // that's too annoying, so open the
					 // popup only in response to
					 // key or button events.
					 switch (trigger.index()) {
					 case x::w::callback_trigger_key_event:
					 case x::w::
						 callback_trigger_button_event:

						 me->appearance_values_popup
							 ->show_all(IN_THREAD);
						 break;
					 }
				   });
		 });

	// Enable/disable Move/Update/Delete/Create appearance values menu
	// popup depending on which value has been selected.
	appearance_values_lm->on_selection_changed
		([]
		 (ONLY IN_THREAD,
		  const auto &status_info)
		 {
			 appinvoke([&]
				   (auto *me)
				   {
					   appearance_info_t::lock
						   lock{me->appearance_info};

					   me->appearance_value_selected
						   (IN_THREAD, lock,
						    status_info);
				   });
		 });

	// Move appearance value up/down/delete.
	appearance_value_move_up->on_status_update
		([]
		 (ONLY IN_THREAD,
		  const auto &status_info)
		 {
			 if (status_info.trigger.index() ==
			     x::w::callback_trigger_initial)
				 return;

			 appinvoke(&appObj::appearance_on_value_move_up,
				   IN_THREAD);
		 });
	appearance_value_move_down->on_status_update
		([]
		 (ONLY IN_THREAD,
		  const auto &status_info)
		 {
			 if (status_info.trigger.index() ==
			     x::w::callback_trigger_initial)
				 return;

			 appinvoke(&appObj::appearance_on_value_move_down,
				   IN_THREAD);
		 });
	appearance_value_update->on_status_update
		([]
		 (ONLY IN_THREAD,
		  const auto &status_info)
		 {
			 if (status_info.trigger.index() ==
			     x::w::callback_trigger_initial)
				 return;

			 appinvoke(&appObj::appearance_value_edit_update,
				   IN_THREAD);
		 });
	appearance_value_delete->on_status_update
		([]
		 (ONLY IN_THREAD,
		  const auto &status_info)
		 {
			 if (status_info.trigger.index() ==
			     x::w::callback_trigger_initial)
				 return;
			 appinvoke(&appObj::appearance_on_value_delete,
				   IN_THREAD);
		 });

	 // Save our elements
	elements.appearance_name=appearance_name;
	elements.appearance_new_name_label=appearance_new_name_label;
	elements.appearance_new_name=appearance_new_name;
	elements.appearance_new_name_label2=appearance_new_name_label2;
	elements.appearance_new_type=appearance_new_type;
	elements.appearance_new_name_label3=appearance_new_name_label3;
	elements.appearance_new_create=appearance_new_create;
	elements.appearance_contents_page=appearance_contents_page;
	elements.appearance_based_on_field=appearance_based_on_field;
	elements.appearance_values=appearance_values;
	elements.appearance_values_popup=appearance_values_popup;
	elements.appearance_update_button=appearance_update_button;
	elements.appearance_delete_button=appearance_delete_button;
	elements.appearance_reset_button=appearance_reset_button;
	elements.appearance_value_move_up=appearance_value_move_up;
	elements.appearance_value_move_down=appearance_value_move_down;
	elements.appearance_value_update=appearance_value_update;
	elements.appearance_value_create=appearance_value_create;
	elements.appearance_value_delete=appearance_value_delete;

	// Appearance create/update/delete/reset

	appearance_new_create->on_activate
		([]
		 (ONLY IN_THREAD,
		  const auto &trigger,
		  const auto &busy)
		 {
			 appinvoke(&appObj::update_theme,
				   IN_THREAD, busy,
				   &appObj::appearance_create);
		 });

	appearance_update_button->on_activate
		([]
		 (ONLY IN_THREAD,
		  const auto &trigger,
		  const auto &busy)
		 {
			 appinvoke(&appObj::update_theme,
				   IN_THREAD, busy,
				   &appObj::appearance_update);
		 });

	appearance_delete_button->on_activate
		([]
		 (ONLY IN_THREAD,
		  const auto &trigger,
		  const auto &busy)
		 {
			 appinvoke(&appObj::update_theme,
				   IN_THREAD, busy,
				   &appObj::appearance_delete);
		 });

	appearance_reset_button->on_activate
		([]
		 (ONLY IN_THREAD,
		  const auto &trigger,
		  const auto &busy)
		 {
			 appinvoke(&appObj::appearance_reset, IN_THREAD);
		 });

	// Delete button pressed on dialog window.
	elements.appearance_dialog->dialog_window
		->on_delete([]
			    (ONLY IN_THREAD,
			     const auto &ignore)
			    {
				    appinvoke([&]
					      (appObj *me)
					      {
						      me->appearance_dialog
							      ->dialog_window
							      ->hide(IN_THREAD);
					      });
			    });
}

void appObj::appearances_initialize(ONLY IN_THREAD)
{
	appearance_info_t::lock lock{appearance_info};

	// Inventory all appearances.
	auto existing_appearances=theme.get()->readlock();

	existing_appearances->get_root();
	auto xpath=existing_appearances->get_xpath("/theme/appearance");
	auto n=xpath->count();

	lock->ids.clear();
	lock->ids.reserve(n);

	for (size_t i=0; i<n; ++i)
	{
		xpath->to_node(i+1);

		lock->ids.push_back(existing_appearances->get_any_attribute("id"));
	}

	std::sort(lock->ids.begin(), lock->ids.end());

	// Duplicate IDS: TODO - report this.

	lock->ids.erase(std::unique(lock->ids.begin(), lock->ids.end()),
			lock->ids.end());

	// For the appearance_name combo-box, put "-- New Appearance--",
	// followed by all loaded appearances.
	std::vector<x::w::list_item_param> combobox_items;

	combobox_items.reserve(lock->ids.size()+1);

	combobox_items.push_back(_("-- New Appearance --"));
	combobox_items.push_back(_(""));

	for (const auto &name:lock->ids)
	{
		auto new_element=appearance_name_and_description(name);

		combobox_items.insert(combobox_items.end(),
				      new_element.description.begin(),
				      new_element.description.end());
	}

	auto lm=appearance_name->standard_comboboxlayout();

	lm->replace_all_items(IN_THREAD, combobox_items);

	// The autoselect will initialize the rest.
	lm->autoselect(IN_THREAD, 0, {});
}

appObj::new_element_t
appObj::appearance_name_and_description(const std::string &n)
{
	auto this_appearance=theme.get()->readlock();

	this_appearance->get_root();

	get_xpath_for(this_appearance, "appearance", n)->to_node(1);

	return {n, {n, appearance_current_value_type(this_appearance)
			->first}};
}

void appObj::appearance_fv::format(std::vector<x::w::list_item_param> &v) const
{
	v.push_back(field);
	std::visit(x::visitor{
			[&](const reset &)
			{
				   x::w::text_param t;

				   t("list; weight=bold"_theme_font);
				   t(_("(reset)"));
				   v.push_back(t);
			}, [&](const std::u32string &s)
			   {
				   x::w::text_param t;

				   t("list"_theme_font);
				   t(s);
				   v.push_back(t);

			   }},value);
}

void appObj::appearance_reset(ONLY IN_THREAD)
{
	appearance_info_t::lock lock{appearance_info};

	lock->current_selection.reset();

	auto name_lm=appearance_name->standard_comboboxlayout();

	auto s=name_lm->selected();

	auto appearance_contents_page_lm=
		appearance_contents_page->pagelayout();

	if (!s || *s == 0)
	{
		// Show fields for creating new appearances.

		appearance_new_name_label->show(IN_THREAD);
		appearance_new_name->show(IN_THREAD);
		appearance_new_name_label2->show(IN_THREAD);
		appearance_new_type->show(IN_THREAD);
		appearance_new_name_label3->show(IN_THREAD);
		appearance_new_create->show(IN_THREAD);

		// Close the page with the existing appearance value contents.
		appearance_contents_page_lm->close();

		// Must still initialize appearance_values to a non-empty
		// list, in order to have its metrics factored in.
		std::vector<x::w::list_item_param> list_values;

		appearance_fv fv("",U"");
		fv.format(list_values);

		appearance_values->tablelayout()
			->replace_all_items(IN_THREAD, list_values);
	}
	else
	{
		size_t &n=*s;

		// Hide new appearance fields.
		appearance_new_name_label->hide(IN_THREAD);
		appearance_new_name->set(IN_THREAD, "");
		appearance_new_name->hide(IN_THREAD);
		appearance_new_name_label2->hide(IN_THREAD);
		appearance_new_type->hide(IN_THREAD);
		appearance_new_name_label3->hide(IN_THREAD);
		appearance_new_create->hide(IN_THREAD);

		// Show this page.
		x::w::uielements ignore;

		appearance_contents_page_lm->generate("appearance_values_show",
						      current_generators
						      ->cxxwui_generators,
						      ignore);

		// Reset current selection values.

		lock->current_selection.emplace();

		auto &current_selection=
			*lock->current_selection;

		current_selection.index=n-1;

		auto current_value=appearance_current_value(lock);

		lock->current_selection->from=
			current_value->get_any_attribute("from");

		lock->current_fields.clear();
		std::vector<std::tuple<std::string,
				       x::xml::readlock>> current_field_names;

		lock->current_defaults.clear();
		size_t current_default=0;

		auto type_iter=appearance_current_value_type(current_value);

		if (type_iter != appearance_types.end()) // Else shouldn't
		{
			auto value=type_iter->second->clone();

			// Grab all the default values

			auto xpath=value->get_xpath("default");

			size_t n=xpath->count();

			lock->current_defaults.reserve(n);

			for (size_t i=1; i <= n; ++i)
			{
				xpath->to_node(i);

				auto s=value->get_text();
				lock->current_defaults.push_back(s);

				// The initial value in the from combo-box
				// is an empty placeholder. This 1-based
				// for-loop ideally sets current_default to the
				// right value.

				if (s == lock->current_selection->from)
					current_default=i;
			}

			// Possible fields
			value=type_iter->second->clone();
			xpath=value->get_xpath("field");

			n=xpath->count();

			// Save the names of the fields separately.
			current_field_names.reserve(n);

			for (size_t i=1; i <= n; ++i)
			{
				xpath->to_node(i);
				auto f=value->clone();
				f->get_xpath("name")->to_node();

				current_field_names
					.emplace_back(f->get_text(),
						      value->clone());
			}
		}

		// Populate the submenu for "Create", listing all fields
		// that can be created. We also record some things about
		// each field, i.e. whether it's multiple or optional.
		//
		// First, we gather metadata about each field.

		for (const auto &name_and_descr:current_field_names)
		{
			auto &[name, value] = name_and_descr;

			auto f=value->clone();
			f->get_xpath("descr")->to_node();
			auto descr=f->get_text();

			f=value->clone();
			f->get_xpath("type|ref")->to_node();
			auto fname=f->name();
			auto type=f->get_text();

			bool reset=false;
			bool multiple=false;

			f=value->clone();
			if (f->get_xpath("vector")->count())
			{
				reset=multiple=true;
			}
			f=value->clone();
			if (f->get_xpath("optional")->count())
				reset=true;

			lock->current_fields.emplace
				(name, appearance_field
				 {
					 fname + "[" + type + "]",
						 descr,
						 multiple,
						 reset
						 });
		}

		// Now we're ready to populate the "Create" submenu.

		std::vector<x::w::list_item_param> current_field_name_items;

		current_field_name_items.reserve(current_field_names.size()*2);

		for (const auto &current_field:lock->current_fields)
		{
			auto &name=current_field.first;

			current_field_name_items.emplace_back
				([name]
				 (ONLY IN_THREAD,
				  const auto &status_info)
				 {
					 if (status_info.trigger.index() ==
					     x::w::callback_trigger_initial)
						 return;

					 appinvoke(&appObj::
						   appearance_value_edit_create,
						   IN_THREAD,
						   name);
				 });
			current_field_name_items.emplace_back(name);
		}

		appearance_value_create->submenu_listlayout()
			->replace_all_items(IN_THREAD,
					    current_field_name_items);

		// Initialize the "From" combo-box.

		std::vector<x::w::list_item_param> list_values;

		list_values.reserve(lock->current_defaults.size()+1);

		list_values.push_back("");

		list_values.insert(list_values.end(),
				   lock->current_defaults.begin(),
				   lock->current_defaults.end());

		auto appearance_based_on=
			appearance_based_on_field->standard_comboboxlayout();

		appearance_based_on->replace_all_items(IN_THREAD,
						       list_values);
		appearance_based_on->autoselect(IN_THREAD, current_default,
						{});

		auto utf8=x::locale::base::utf8();

		// Load the appearance values into current_selection.values

		current_selection.values.reserve
			(current_value->get_child_element_count());
		for (auto flag=current_value->get_first_element_child(); flag;
		     flag=current_value->get_next_element_sibling())
		{
			bool do_reset=false;

			if (current_value->get_child_element_count())
			{
				auto xpath=current_value->get_xpath("reset");

				if (xpath->count())
					do_reset=true;
			}

			current_selection.values
				.emplace_back(current_value->name(),
					      do_reset ?
					      appearance_value_t{reset{}}
					      : appearance_value_t
					      {utf8->tou32
						       (current_value
							->get_text())});
		}

		// Initial state, unchanged.
		lock->save_params=appearance_save_params{};
		lock->save_params.from=current_selection.from;
		lock->save_params.values=current_selection.values;

		// Initialize the table with the current values.

		list_values.clear();
		list_values.reserve(current_selection.values.size()*2+2);
		for (const auto &v:current_selection.values)
		{
			v.format(list_values);
		}

		// Add "append" at the end.

		{
			x::w::text_param t;

			t("list;slant=italic"_theme_font);
			t(_("(append)"));

			list_values.push_back(t);
			list_values.push_back("");
		}
		appearance_values->tablelayout()
			->replace_all_items(IN_THREAD, list_values);
	}

	appearance_enable_disable_buttons(IN_THREAD, lock);
	appearance_enable_disable_new_fields(IN_THREAD, lock);
}

x::xml::readlock appObj::appearance_current_value(appearance_info_t::lock &lock)
{
	auto &current_selection=lock->current_selection.value();

	auto current_value=theme.get()->readlock();

	current_value->get_root();

	auto xpath=get_xpath_for(current_value,
				 "appearance",
				 lock->ids.at(current_selection.index));

	// We expect one to be there, of course.
	xpath->to_node(1);

	return current_value;
}

appObj::appearance_types_t::const_iterator
appObj::appearance_current_value_type(appearance_info_t::lock &lock)
{
	return appearance_current_value_type(appearance_current_value(lock));
}

appObj::appearance_types_t::const_iterator
appObj::appearance_current_value_type(const x::xml::readlock &current_value)
{
	return appearance_types.find(current_value->get_any_attribute("type"));
}

void appObj::new_appearance_type_selected
(ONLY IN_THREAD,
 const x::w::standard_combobox_selection_changed_info_t &info)
{
	if (!info.list_item_status_info.selected)
		return;

	appearance_info_t::lock lock{appearance_info};

	appearance_enable_disable_buttons(IN_THREAD, lock);
}

std::string appObj::get_new_appearance_name(appearance_info_t::lock &)
{
	return x::w::input_lock{appearance_new_name}.get();
}

void appObj::appearance_enable_disable_buttons(ONLY IN_THREAD,
					       appearance_info_t::lock &lock)
{
	// Enable or disable the "Create" new appearance button.

	auto new_appearance_name=get_new_appearance_name(lock);

	auto appearance_type_lm=
		appearance_new_type->standard_comboboxlayout();

	// Capture which new appearance type has been selected (if any)

	auto appearance_type_selected=appearance_type_lm->selected();

	lock->new_appearance_type.clear();

	if (!lock->current_selection && appearance_type_selected)
	{
		// Make a copy of the cached appearance type names, the
		// combo-box entries are in the same order.
		std::vector<std::string> appearance_type_list;

		appearance_type_list.reserve(appearance_types.size());

		for (const auto &type:appearance_types)
			appearance_type_list.push_back(type.first);

		lock->new_appearance_type=appearance_type_list
			.at(*appearance_type_selected);
	}

	// The "Create" button is enabled if the new appearance name is not
	// empty and the type is selected.

	appearance_new_create->set_enabled(!new_appearance_name.empty() &&
					   !lock->new_appearance_type.empty());

	// Update existing appearance.

	{
		auto appearance_based_on=
			appearance_based_on_field->standard_comboboxlayout();

		auto based_on=appearance_based_on->selected();

		if (based_on)
		{
			if (*based_on)
				lock->save_params.from=
					lock->current_defaults.at(*based_on-1);
			else
				lock->save_params.from.clear();
		}
	}

	bool modified;

	modified=false;

	if (lock->current_selection &&
	    lock->save_params == *lock->current_selection)
	{
		// Currently shown appearance is unchanged. We can
		// enable the appearance name combo-box, to switch to
		// another appearance. The delete button is enabled, the
		// currently shown appearance can be deleted. Update and
		// Reset are disabled, nothing to update or reset.

		appearance_name->set_enabled(IN_THREAD, true);

		appearance_delete_button->set_enabled(IN_THREAD, true);
		appearance_reset_button->set_enabled(IN_THREAD, false);
		appearance_update_button->set_enabled(IN_THREAD, false);
	}
	else
	{
		// Either no appearance is currently selected, or the
		// shown appearance values were edited and not saved, in
		// which case the appearance name combo-box is disabled.
		//
		// The delete/reset/update buttons are on the page that's
		// shown only when a current appearance is selected, so their
		// status can be set in all cases.

		appearance_name->set_enabled(IN_THREAD,
					     lock->current_selection ?
					     false:true);

		appearance_delete_button->set_enabled(IN_THREAD, false);
		appearance_reset_button->set_enabled(IN_THREAD, true);
		appearance_update_button->set_enabled(IN_THREAD, true);
		//
		// If an appearance if selected, it must've been modified
		//
		if (lock->current_selection)
			modified=true;
	}

	update([&]
	       (auto &info)
	{
		info.modified=modified;
	});
}

void appObj::appearance_enable_disable_new_fields(ONLY IN_THREAD,
						  appearance_info_t::lock &lock)
{
	// Compile a list of values that are already set.
	//
	// In the "Create" submenu we will disable all fields that are already
	// set, unless they're multiple fields.

	std::unordered_set<std::string> existing_field_values;

	for (const auto &value:lock->save_params.values)
		existing_field_values.insert(value.field);

	auto create_field_submenu=appearance_value_create->submenu_listlayout();

	create_field_submenu->notmodified();

	// This must be in the same order as current_fields;

	size_t i=0;

	for (const auto &[name, field_type]:lock->current_fields)
	{
		if (!field_type.multiple && existing_field_values.find(name)
		    != existing_field_values.end())
		{
			// This field can occur only once, and it already is

			create_field_submenu->enabled(IN_THREAD, i, false);
		}
		else
		{
			create_field_submenu->enabled(IN_THREAD, i, true);
		}
		++i;
	}
}

void appObj::appearance_value_selected(ONLY IN_THREAD,
				      appearance_info_t::lock &lock,
				      const x::w::list_item_status_info_t &info)
{
	// Navigating the appearance values table.

	if (!info.selected)
	{
		// Deselected, the show is off.
		lock->save_params.current_value.reset();
		appearance_value_move_up->enabled(IN_THREAD, false);
		appearance_value_move_down->enabled(IN_THREAD, false);
		appearance_value_update->enabled(IN_THREAD, false);
		appearance_value_create->enabled(IN_THREAD, false);
		appearance_value_delete->enabled(IN_THREAD, false);
		appearance_values_popup->hide(IN_THREAD);
	}
	else
	{
		size_t n=lock->save_params.values.size();

		// Can move up unless on the first row or on the "create"
		// row.
		appearance_value_move_up->enabled
			(IN_THREAD,
			 info.item_number < n &&
			 info.item_number > 0);

		// Can move down if there's something below is.
		appearance_value_move_down->enabled
			(IN_THREAD,
			 info.item_number+1 < lock->save_params.values.size());

		// Update and delete is possible if not on "create".

		if (info.item_number < n)
		{
			appearance_value_update->enabled(IN_THREAD, true);
			appearance_value_delete->enabled(IN_THREAD, true);
			lock->save_params.current_value=info.item_number;
		}
		else
		{
			appearance_value_update->enabled(IN_THREAD, false);
			appearance_value_delete->enabled(IN_THREAD, false);
		}

		// Create is always enabled.
		appearance_value_create->enabled(IN_THREAD, true);
	}
}

namespace {
#if 0
}
#endif

// appearance_on_value_move_up/down share common logic with handling
// what gets moved up or down:
//
// If an item is explicitly selected, use that, otherwise use what's currently
// highlighted (by keyboard navigation, most likely). This avoids having to
// explicitly hit Enter beforing moving this value.
//
// The constructor figures out what the currently-selected value is.

struct appearance_on_value_index : std::optional<size_t> {

	const x::w::tablelayoutmanager lm;

	using std::optional<size_t>::operator=;
	using std::optional<size_t>::operator bool;
	using std::optional<size_t>::operator*;

	//! Constructor.

	appearance_on_value_index(const x::w::tablelayoutmanager &lm,
				  std::optional<size_t> &current_value,
				  size_t values_size)
		: std::optional<size_t>{lm->current_list_item()},
		  lm{lm}
	{
		if (current_value)
			*this=*current_value;

		// The currently-highlighted list item could be the last item
		// which is not movable.

		if (*this &&
		    **this >= values_size)
			reset();
	}

	//! After everything is moved, current_value must be updated.
	//!
	//! But only if it's where the current value came from.

	void update(ONLY IN_THREAD, size_t n)
	{
		lm->autoselect(IN_THREAD, n, {});
	}
};

#if 0
{
#endif
}

void appObj::appearance_on_value_move_up(ONLY IN_THREAD)
{
	appearance_info_t::lock lock{appearance_info};

	auto tlm=appearance_values->tablelayout();

	appearance_on_value_index new_value_index{tlm,
		lock->save_params.current_value,
		lock->save_params.values.size()};

	if (!new_value_index)
		return;

	auto &n=*new_value_index;

	if (n == 0)
		return; // Shouldn't happen.

	// Make a copy of the preceding item.
	std::vector<x::w::list_item_param> moved_item;

	moved_item.reserve(2);

	lock->save_params.values.at(n-1).format(moved_item);

	// Swap this value with the preceding one.
	std::swap(lock->save_params.values.at(n),
		  lock->save_params.values.at(n-1));

	// Remove the preceding one then use the duplicate copy to re-insert
	// it after the current item.
	tlm->remove_item(IN_THREAD, n-1);
	tlm->insert_items(IN_THREAD, n, moved_item);

	new_value_index.update(IN_THREAD, --n);

	appearance_enable_disable_buttons(IN_THREAD, lock);
}

void appObj::appearance_on_value_move_down(ONLY IN_THREAD)
{
	appearance_info_t::lock lock{appearance_info};

	auto tlm=appearance_values->tablelayout();

	appearance_on_value_index new_value_index{tlm,
		lock->save_params.current_value,
		lock->save_params.values.size()};

	if (!new_value_index)
		return;

	auto &n=*new_value_index;

	if (n+1 >= lock->save_params.values.size())
		return;

	// Make a duplicate copy of the next item.

	std::vector<x::w::list_item_param> moved_item;

	moved_item.reserve(2);

	lock->save_params.values.at(n+1).format(moved_item);

	// Swap the values
	std::swap(lock->save_params.values.at(n),
		  lock->save_params.values.at(n+1));

	// Remove the next item, then use the duplicate copy to re-insert it
	// before this item.
	tlm->remove_item(IN_THREAD, n+1);
	tlm->insert_items(IN_THREAD, n, moved_item);

	new_value_index.update(IN_THREAD, ++n);

	appearance_enable_disable_buttons(IN_THREAD, lock);
}

namespace {
#if 0
}
#endif

// Helpers for creating the new value widget, either an editable or a
// standard combo-box.

struct create_appearance_value_combobox {

public:
	std::vector<x::w::list_item_param> values;

	const x::w::button save_button;
	const std::string field_name;
	const x::w::image_button appearance_new_value_option;

	template<typename iter_type>
	create_appearance_value_combobox(iter_type b,
					 iter_type e,
					 const x::w::button &save_button,
					 const std::string &field_name,
					 const x::w::image_button
					 &appearance_new_value_option)
		: values{b, e},
		  save_button{save_button},
		  field_name{field_name},
		  appearance_new_value_option{appearance_new_value_option}
	{
	}

	create_appearance_value_combobox(const x::w::button &save_button,
					 const std::string &field_name,
					 const x::w::image_button
					 &appearance_new_value_option)
		: save_button{save_button},
		  field_name{field_name},
		  appearance_new_value_option{appearance_new_value_option}
	{
	}

	typedef appObj::appearance_value_t appearance_value_t;

	void create(const x::w::const_uigenerators &g,
		    const x::w::container &c,
		    const appearance_value_t &v)
	{
		auto layout=c->gridlayout();

		x::w::uielements new_elements;

		new_elements.new_elements.emplace("appearance_new_value_option",
						  appearance_new_value_option);

		layout->generate(layout_name(), g, new_elements);

		x::w::focusable_container fc=
			new_elements.get_element(element_name());

		auto lm=fc->standard_comboboxlayout();

		lm->replace_all_items(values);

		std::optional<size_t> index;

		std::visit(x::visitor{
				[&, this](const std::u32string &s)
				{
					if (s.empty())
						return;

					size_t i=0;

					for (const auto &v:values)
					{
						if (std::holds_alternative<
						    x::w::text_param>(v) &&
						    std::get<x::w::text_param>
						    (v).string == s)
						{
							// If we found the
							// initial value
							// explicitly listed,
							// formally select
							// it in the combo box.

							lm->autoselect(i);
							return;
						}
						i++;
					}

					// Otherwise, we can set it directly,
					// if this is an editable combobox.
					set_value(lm, s);
				},[](appObj::reset)
				{
				},
			}, v);

		install_save_button_callback();
	}

	virtual const char *layout_name()=0;

	virtual const char *element_name()=0;

	virtual void install_save_button_callback()=0;

	virtual void set_value(const x::w::standard_comboboxlayoutmanager &lm,
			       const std::u32string &v)
	{
	}
};

struct create_appearance_value_editable_combobox
	: create_appearance_value_combobox {

	using create_appearance_value_combobox
	::create_appearance_value_combobox;

	const char *layout_name() override
	{
		return "appearance_new_value_editable_combobox_open";
	}

	const char *element_name() override
	{
		return "appearance_new_value_editable_combobox";
	}

	void install_save_button_callback() override
	{
		// Save button calls appearance_new_value_edit_combo

		save_button->on_activate
			([field_name=this->field_name]
			 (ONLY IN_THREAD,
			  const auto &grigger,
			  const auto &busy)
			 {
				 appinvoke(&appObj
					   ::appearance_new_value_edit_combo,
					   IN_THREAD, field_name);
			 });
	}

	void set_value(const x::w::standard_comboboxlayoutmanager &lm,
		       const std::u32string &v) override
	{
		x::w::editable_comboboxlayoutmanager elm=lm;

		elm->set(v);
	}
};

struct create_appearance_value_standard_combobox
	: create_appearance_value_combobox {

	using create_appearance_value_combobox
	::create_appearance_value_combobox;

	const char *layout_name() override
	{
		return "appearance_new_value_standard_combobox_open";
	}

	const char *element_name() override
	{
		return "appearance_new_value_standard_combobox";
	}

	void install_save_button_callback() override;
};

void create_appearance_value_standard_combobox
::install_save_button_callback()
{
	// Now convert the list_item_params to plain text strings

	std::vector<std::u32string> text_values;

	text_values.reserve(values.size());

	for (const auto &list_value:values)
	{
		const x::w::list_item_param::variant_t &v=list_value;

		std::visit(x::visitor{[&](const x::w::text_param &t)
				      {
					      text_values
						      .emplace_back(t.string);
				      },
				      [&](const auto &)
				      {
					      // Separator
					      text_values
						      .push_back(U"");
				      }}, v);
	}

	// Save button calls appearance_new_value_std_combo
	save_button->on_activate
		([field_name=this->field_name,
		  text_values=std::move(text_values)]
		 (ONLY IN_THREAD,
		  const auto &grigger,
		  const auto &busy)
		 {
			 appinvoke(&appObj::appearance_new_value_std_combo,
				   IN_THREAD, field_name, text_values);
		 });
}

#if 0
{
#endif
}

void appObj::appearance_value_edit_create(ONLY IN_THREAD,
					  const std::string &n)
{
	appearance_info_t::lock lock{appearance_info};

	lock->save_params.current_value_update=false;

	appearance_value_edit(IN_THREAD, lock, n, U"");
}

void appObj::appearance_value_edit_update(ONLY IN_THREAD)
{
	appearance_info_t::lock lock{appearance_info};

	lock->save_params.current_value_update=true;

	auto &save_params=lock->save_params;

	size_t n=save_params.values.size();

	if (save_params.current_value &&
	    // Shouldn't be the case otherwise:
	    *save_params.current_value < n)
	{
		auto &[field, value] =
			save_params.values[*save_params.current_value];
		lock->save_params.current_value_update=true;
		appearance_value_edit(IN_THREAD, lock, field, value);
	}
}

void appObj::appearance_value_edit(ONLY IN_THREAD,
				   appearance_info_t::lock &lock,
				   const std::string &n,
				   const appearance_value_t &v)
{
	if (!lock->current_selection)
		return;

	auto type_iter=appearance_current_value_type(lock);

	if (type_iter == appearance_types.end())
		return;

	auto type_lock=type_iter->second->clone();

	// Find the value's <type> or <ref>
	type_lock->get_xpath("field[name='" + n + "']/type | "
			     "field[name='" + n + "']/ref")->to_node();

	auto category=type_lock->name();

	std::string type;
	std::string ref;

	*(category == "ref" ? &ref:&type)=type_lock->get_text();

	type_lock->get_xpath("../descr")->to_node();

	auto description=x::trim(type_lock->get_text());

	// Some massaging of Doxygen markup.

	{
		auto b=description.begin(), p=b, e=description.end();

		while (b != e)
		{
			if (*b != '\\')
			{
				*p++=*b++;
				continue;
			}

			// Collect the next word.
			std::string word;

			++b;

			while (b != e)
			{
				if (*b < 'a' || *b > 'z')
					break;

				word += *b++;
			}

			// "\note" gets replaced by "NOTE:", same # of chars.

			if (word == "note")
			{
				*p++='N';
				*p++='O';
				*p++='T';
				*p++='E';
				continue;
			}

			// Drop the ref and the referenced anchor.

			if (word == "ref")
			{
				// Skip spaces after \ref
				while (b != e)
				{
					if (*b != ' ')
						break;
					++b;
				}

				// Look for next space (skip the anchor)
				while (b != e)
				{
					if (*b == ' ')
						break;
					++b;
				}
				continue;
			}

			continue;
		}

		description.erase(p, e);
	}

	// The description has hard newline at the end of each line.
	// Replace them with spaces.

	{
		size_t newline=0;

		auto p=description.begin();
		auto e=description.end();

		for (auto b=p; b != e; ++b)
		{
			if (*b != '\n')
			{
				if (newline > 1)
				{
					*p++='\n';
					*p++='\n';
				}
				else if (newline)
					*p++=' ';

				newline=0;
				*p++=*b;
			}
			else
				++newline;
		}

		description.erase(p, e);
	}

	// Replace any consecutive spaces with a single space.

	{
		bool newline=true;
		bool space=false;
		bool alpha=false;

		auto p=description.begin();
		auto e=description.end();
		auto word_start=p;

		for (auto b=p; b != e; ++b)
		{
			if ( (*b >= 'a' && *b <= 'z') ||
			     (*b >= 'A' && *b <= 'Z') ||
			     *b == '_')
			{
				if (!alpha)
					word_start=p;
				alpha=true;
			}
			else
			{
				if (alpha &&
				    std::string{word_start, p} ==
				    "INSERT_LIBX_NAMESPACE")
				{
					*word_start++=LIBCXX_NAMESPACE_STR[0];
					p=word_start;
				}
				alpha=false;
			}

			if (*b == '\n')
			{
				newline=true;
				space=false;
				*p++=*b;
				continue;
			}

			if (*b == ' ')
			{
				if (newline)
					continue; // Beginning of line.
				space=true;
				continue;
			}
			if (space)
				*p++=' ';

			newline=false;
			space=false;
			*p++=*b;
		}
		description.erase(p, e);
	}

	appearance_new_value_description->update(description);

	bool initialized=false;

	bool reset_option=false;
	bool reset_option_selected=false;

	{
		auto xpath=type_lock->get_xpath("../optional | ../vector");

		if (xpath->count() == 1)
		{
			xpath->to_node();
			if (type_lock->get_text() != "0")
			{
				reset_option=true;
				reset_option_selected=
					std::holds_alternative<reset>(v);
			}
		}
	}

	// If the value is another appearance, put a list of possibilities
	// into the combo-box dropdown.

	if (!ref.empty())
	{
		std::vector<x::w::list_item_param> list;

		// Start with the built-in appearance defaults.

		auto t=appearance_types.find(ref);

		std::vector<std::string> defaults;

		if (t != appearance_types.end())
		{
			auto lock=t->second->clone();

			auto xpath=lock->get_xpath("default");

			size_t n=xpath->count();

			for (size_t i=1; i <= n; ++i)
			{
				xpath->to_node(i);

				defaults.push_back(lock->get_text());
			}
		}

		std::sort(defaults.begin(), defaults.end());

		list.insert(list.end(), defaults.begin(), defaults.end());

		// Follow it with a separator, and any custom appearances
		// that we already defined.

		list.emplace_back(x::w::separator{});

		defaults.clear();

		auto existing_appearances=theme.get()->readlock();

		existing_appearances->get_root();
		auto xpath=existing_appearances
			->get_xpath("/theme/appearance[@type='" + ref + "']");

		auto j=xpath->count();
		for (size_t i=1; i <= j; ++i)
		{
			xpath->to_node(i);

			defaults.push_back
				(existing_appearances->get_any_attribute("id"));
		}
		std::sort(defaults.begin(), defaults.end());

		list.insert(list.end(), defaults.begin(), defaults.end());

		create_appearance_value_editable_combobox create
			{
			 list.begin(),
			 list.end(),
			 appearance_new_value_save,
			 n,
			 appearance_new_value_option,
			};

		create.create(appearance_dialog_gen,
			      appearance_new_value_container, v);
		initialized=true;
	}
	else if (type == "dim_arg")
	{
		dimension_info_t::lock lock{dimension_info};

		create_appearance_value_editable_combobox create
			{
			 lock->ids.begin(),
			 lock->ids.end(),
			 appearance_new_value_save,
			 n,
			 appearance_new_value_option,
			};

		create.create(appearance_dialog_gen,
			      appearance_new_value_container, v);
		initialized=true;
	}
	else if (type == "color_arg" || type == "text_color_arg")
	{
		colors_info_t::lock lock{colors_info};

		create_appearance_value_editable_combobox create
			{
			 appearance_new_value_save,
			 n,
			 appearance_new_value_option,
			};

		create.values.reserve(lock->ids.size()+1+
				      x::w::n_rgb_colors);

		create.values.insert(create.values.end(),
				     x::w::rgb_color_names,
				     x::w::rgb_color_names+
				     x::w::n_rgb_colors);

		create.values.emplace_back(x::w::separator{});

		create.values.insert(create.values.end(),
				     lock->ids.begin(),
				     lock->ids.end());

		create.create(appearance_dialog_gen,
			      appearance_new_value_container, v);

		initialized=true;
	}
	else if (type == "border_arg")
	{
		border_info_t::lock lock{border_info};

		create_appearance_value_editable_combobox create
			{
			 lock->ids.begin(),
			 lock->ids.end(),
			 appearance_new_value_save,
			 n,
			 appearance_new_value_option,
			};

		create.create(appearance_dialog_gen,
			      appearance_new_value_container, v);
		initialized=true;
	}
	else if (type == "font_arg")
	{
		font_info_t::lock lock{font_info};

		create_appearance_value_editable_combobox create
			{
			 lock->ids.begin(),
			 lock->ids.end(),
			 appearance_new_value_save,
			 n,
			 appearance_new_value_option,
			};

		create.create(appearance_dialog_gen,
			      appearance_new_value_container, v);
		initialized=true;
	}

	if (type == "halign")
	{
		create_appearance_value_standard_combobox create
			{
			 std::begin(x::w::halign_names),
			 std::end(x::w::halign_names),
			 appearance_new_value_save,
			 n,
			 appearance_new_value_option,
			};

		create.create(appearance_dialog_gen,
			      appearance_new_value_container, v);

		initialized=true;
	}
	else if (type == "valign")
	{
		create_appearance_value_standard_combobox create
			{
			 std::begin(x::w::valign_names),
			 std::end(x::w::valign_names),
			 appearance_new_value_save,
			 n,
			 appearance_new_value_option,
			};

		create.create(appearance_dialog_gen,
			      appearance_new_value_container, v);

		initialized=true;
	}
	else if (type == "scrollbar_visibility")
	{
		create_appearance_value_standard_combobox create
			{
			 std::begin(x::w::scrollbar_visibility_names),
			 std::end(x::w::scrollbar_visibility_names),
			 appearance_new_value_save,
			 n,
			 appearance_new_value_option,
			};

		create.create(appearance_dialog_gen,
			      appearance_new_value_container, v);
		initialized=true;
	}

	if (!initialized)
	{
		x::w::uielements new_elements;

		new_elements.new_elements.emplace("appearance_new_value_option",
						  appearance_new_value_option);
		auto layout=appearance_new_value_container->gridlayout();

		layout->generate("appearance_new_value_input_field_open",
				 appearance_dialog_gen, new_elements);

		x::w::input_field field=
			appearance_new_value_container->gridlayout()->get(0, 0);

		if (std::holds_alternative<std::u32string>(v))
		{
			field->set(IN_THREAD, std::get<std::u32string>(v));
		}
		// Save button calls appearance_new_value_input
		appearance_new_value_save->on_activate
			([n]
			 (ONLY IN_THREAD,
			  const auto &grigger,
			  const auto &busy)
			 {
				 appinvoke(&appObj::appearance_new_value_input,
					   IN_THREAD, n);
		 });
	}

	appearance_dialog->dialog_window->set_window_title(n);

	// Enable or disable the reset option, as appropriate, then
	// show_all().
	appearance_reset_value_option->set_enabled(IN_THREAD, reset_option);

	// Figure out which one of the two radio buttons should be selected.
	(reset_option_selected
	 ? appearance_reset_value_option
	 : appearance_new_value_option)->set_value(IN_THREAD, 1);

	appearance_dialog->dialog_window->show_all(IN_THREAD);
}

void appObj::appearance_new_value_std_combo(ONLY IN_THREAD,
					    const std::string &field_name,
					    const std::vector<std::u32string>
					    &v)
{
	x::w::focusable_container combo=
		appearance_new_value_container->gridlayout()->get(0, 0);

	auto index=combo->standard_comboboxlayout()->selected();

	if (index && *index < v.size())
		appearance_new_value_save_and_hide(IN_THREAD, field_name,
						   v[*index]);
}

void appObj::appearance_new_value_edit_combo(ONLY IN_THREAD,
					     const std::string &field_name)
{
	x::w::focusable_container combo=
		appearance_new_value_container->gridlayout()->get(0, 0);

	appearance_new_value_save_and_hide
		(IN_THREAD, field_name,
		 combo->editable_combobox_get_unicode());
}

void appObj::appearance_new_value_input(ONLY IN_THREAD,
					const std::string &field_name)
{
	x::w::input_field field=
		appearance_new_value_container->gridlayout()->get(0, 0);

	appearance_new_value_save_and_hide(IN_THREAD, field_name,
					   x::w::input_lock{field}
					   .get_unicode());
}

void appObj::appearance_new_value_save_and_hide(ONLY IN_THREAD,
						const std::string &field_name,
						const std::u32string &value)
{
	appearance_info_t::lock lock{appearance_info};

	// Construct the new appearance_fv

	appearance_fv new_value{field_name, reset{}};

	if (appearance_new_value_option->get_value())
	{
		new_value.value=value;
	}

	// Figure out where it goes:

	auto &save_params=lock->save_params;

	size_t n=save_params.values.size();

	if (save_params.current_value &&
	    // Shouldn't be the case otherwise:
	    *save_params.current_value < n)
	{
		n=*save_params.current_value;

		if (save_params.current_value_update == true)
			// Updating an existing value
			save_params.values[*save_params.current_value]=
				new_value;
		else
			// Inserting a new value.
			save_params.values.insert(save_params.values.begin()
						  + n,
						  new_value);
	}
	else
	{
		save_params.values.push_back(new_value);

		// Guard against gremlins.
		save_params.current_value_update=false;
	}

	// Update the table with the values.

	std::vector<x::w::list_item_param> new_value_item;

	new_value.format(new_value_item);

	auto lm=appearance_values->tablelayout();

	if (save_params.current_value_update)
	{
		lm->replace_items(IN_THREAD, n, new_value_item);
	}
	else
	{
		lm->insert_items(IN_THREAD, n, new_value_item);
	}

	lm->autoselect(IN_THREAD, n, {});

	// Hide the dialog, and update the new fields submenu, to reflect
	// that there is a new field value in the appearance, and unless the new
	// field allows multiple values we'll disable it here.

	appearance_dialog->dialog_window->hide(IN_THREAD);

	appearance_enable_disable_buttons(IN_THREAD, lock);
	appearance_enable_disable_new_fields(IN_THREAD, lock);
}

void appObj::appearance_value_option_selected(ONLY IN_THREAD, size_t n)
{
	// "Value" radio button selected enables the widget for the new
	// input value.
	//
	// "Reset" radio button option disables it.

	x::w::focusableptr field=
		appearance_new_value_container->gridlayout()->get(0, 0);

	if (!field) // Dialog being created.
		return;

	field->set_enabled(IN_THREAD, n != 0);
}

void appObj::appearance_on_value_delete(ONLY IN_THREAD)
{
	appearance_info_t::lock lock{appearance_info};

	auto &save_params=lock->save_params;

	if (!save_params.current_value ||
	    // Shouldn't happen
	    *save_params.current_value > save_params.values.size())
		return;

	// Which value to remove.
	auto n= *save_params.current_value;

	// Remove it from out values list, and from the table.
	save_params.values.erase(save_params.values.begin() + n);

	auto layout=appearance_values->tablelayout();

	layout->remove_item(IN_THREAD, n);

	// Clear the current_value and call autoselect() again.
	//
	// There's always a (create) entry at the end of the field list,
	// so this will always work.
	save_params.current_value.reset();
	layout->autoselect(IN_THREAD, n, {});

	// And update the new items submenu
	appearance_enable_disable_buttons(IN_THREAD, lock);
	appearance_enable_disable_new_fields(IN_THREAD, lock);
}

appObj::get_updatecallbackptr appObj::appearance_create(ONLY IN_THREAD)
{
	appearance_info_t::lock lock{appearance_info};

	return [saved_lock=lock.threadlock(x::ref{this})]
		(appObj *me)
	       {
		       appearance_info_t::lock lock{saved_lock};

		       return me->appearance_create2(lock);
	       };
}

appObj::update_callback_t appObj::appearance_create2(appearance_info_t::lock
						     &lock)
{
	update_callback_t ret;

	auto new_appearance_name=get_new_appearance_name(lock);

	if (new_appearance_name.empty() ||
	    lock->new_appearance_type.empty())
		return ret;

	auto created=create_update("appearance",
				   new_appearance_name, true);

	if (!created)
		return ret;

	auto &[doc_lock, new_appearance]=*created;

	doc_lock->attribute({"type", lock->new_appearance_type});

	ret.emplace(doc_lock,
		    [=, saved_lock=lock.threadlock(x::ref{this})]
		    (appObj *me,
		     ONLY IN_THREAD,
		     const x::ref<x::obj> &busy_mcguffin)
		    {
			    appearance_info_t::lock lock{saved_lock};

			    me->appearance_create3(IN_THREAD,
						   lock,
						   new_appearance_name,
						   busy_mcguffin);
		    });

	return ret;
}

void appObj::appearance_create3(ONLY IN_THREAD,
				appearance_info_t::lock &lock,
				const std::string &new_appearance_name,
				const x::ref<x::obj> &busy_mcguffin)
{
	update_new_element(IN_THREAD,
			   appearance_name_and_description
			   (new_appearance_name), lock->ids,
			   appearance_name);
}

appObj::get_updatecallbackptr appObj::appearance_update(ONLY IN_THREAD)
{
	appearance_info_t::lock lock{appearance_info};

	return [saved_lock=lock.threadlock(x::ref{this})]
		(appObj *me)
	       {
		       appearance_info_t::lock lock{saved_lock};

		       return me->appearance_update2(lock);
	       };
}

appObj::update_callback_t
appObj::appearance_update2(appearance_info_t::lock &lock)
{
	appObj::update_callback_t ret;

	if (!lock->current_selection)
		return ret; // Shouldn't happen.

	auto &current_selection=*lock->current_selection;

	auto id=lock->ids.at(current_selection.index);

	auto &save_params=lock->save_params;

	auto created_update=create_update("appearance", id, false);

	if (!created_update)
		return ret;

	auto &[doc_lock, new_appearance]=*created_update;

	doc_lock->attribute({"type", appearance_current_value(lock)
			     ->get_any_attribute("type")});

	if (!save_params.from.empty())
		doc_lock->attribute({"from", save_params.from});

	for (const auto &v:save_params.values)
	{
		auto n=doc_lock->create_child()->element({v.field});

		std::visit(x::visitor{[&](const reset &)
				      {
					      n->element({"reset"});
				      },
				      [&](const std::u32string &s)
				      {
					      n->text(s);
				      }}, v.value);

		doc_lock->get_parent();
		doc_lock->get_parent();
	}

	ret.emplace(doc_lock,
		    [=, saved_lock=lock.threadlock(x::ref{this})]
		    (appObj *me,
		     ONLY IN_THREAD,
		     const x::ref<x::obj> &busy_mcguffin)
		    {
			    appearance_info_t::lock lock{saved_lock};

			    me->appearance_update3(IN_THREAD,
						   busy_mcguffin);
		    });

	return ret;
}

void appObj::appearance_update3(ONLY IN_THREAD,
				const x::ref<x::obj> &busy_mcguffin)
{
	appearance_reset(IN_THREAD);
	status->update(_("Appearance updated"));
}

appObj::get_updatecallbackptr appObj::appearance_delete(ONLY IN_THREAD)
{
	appearance_info_t::lock lock{appearance_info};

	return [saved_lock=lock.threadlock(x::ref{this})]
		(appObj *me)
	       {
		       appearance_info_t::lock lock{saved_lock};

		       return me->appearance_delete2(lock);
	       };
}

appObj::update_callback_t
appObj::appearance_delete2(appearance_info_t::lock &lock)
{
	update_callback_t ret;

	if (!lock->current_selection)
		return ret;

	// Locate what we need to delete.
	auto index=lock->current_selection->index;
	auto id=lock->ids.at(index);

	auto new_doc=theme.get()->readlock()->clone_document();

	auto doc_lock=new_doc->writelock();

	while (1)
	{
		doc_lock->get_root();

		auto xpath=get_xpath_for(doc_lock, "appearance", id);

		if (xpath->count() <= 0)
			break;

		xpath->to_node(1);
		doc_lock->remove();
	}

	ret.emplace(doc_lock,
		    [=, saved_lock=lock.threadlock(x::ref{this})]
		    (appObj *me,
		     ONLY IN_THREAD,
		     const x::ref<x::obj> &busy_mcguffin)
		    {
			    appearance_info_t::lock lock{saved_lock};

			    me->appearance_delete3(IN_THREAD,
						   lock, index,
						   busy_mcguffin);
		    });

	return ret;
}

void appObj::appearance_delete3(ONLY IN_THREAD,
				appearance_info_t::lock &lock,
				size_t index,
				const x::ref<x::obj> &busy_mcguffin)
{
	// Update the loaded list of appearances we store here,
	// and set the current appearance combo-box dropdown to "New Appearance".
	lock->ids.erase(lock->ids.begin()+index);

	auto name_lm=appearance_name->standard_comboboxlayout();

	name_lm->remove_item(IN_THREAD, index+1);

	lock->current_selection.reset();

	name_lm->autoselect(IN_THREAD, 0, {});

	appearance_name->request_focus(IN_THREAD);
	status->update(_("Deleted"));
}
