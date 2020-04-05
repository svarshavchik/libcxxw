#include "libcxxw_config.h"
#include "creator/app.H"
#include "x/w/uielements.H"
#include "x/w/impl/uixmlparser.H"
#include "x/w/standard_comboboxlayoutmanager.H"
#include "x/w/editable_comboboxlayoutmanager.H"
#include "x/w/input_field_lock.H"
#include "x/w/input_field.H"
#include "x/w/label.H"
#include "x/w/button.H"
#include "messages.H"
#include "catch_exceptions.H"
#include <x/messages.H>
#include <x/strtok.H>
#include <x/weakcapture.H>
#include <x/mpthreadlock.H>

void appObj::appearances_elements_initialize(app_elements_tptr &elements,
					 x::w::uielements &ui,
					 init_args &args)
{
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

	// Initialize
	{
		x::w::standard_comboboxlayoutmanager appearance_type_lm=
			appearance_new_type->get_layoutmanager();

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

	// Install callbacks.

	appearance_new_name->on_validate
		([]
		 (ONLY IN_THREAD,
		  const auto &trigger)
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

			 return true;
		 });

	x::w::standard_comboboxlayoutmanager appearance_name_lm=
		appearance_name->get_layoutmanager();

	appearance_name_lm->on_selection_changed
		([]
		 (ONLY IN_THREAD,
		  const auto &info)
		 {
			 appinvoke(&appObj::appearance_selected, IN_THREAD,
				   info);
		 });

	// Save our elements
	elements.appearance_name=appearance_name;
	elements.appearance_new_name_label=appearance_new_name_label;
	elements.appearance_new_name=appearance_new_name;
	elements.appearance_new_name_label2=appearance_new_name_label2;
	elements.appearance_new_type=appearance_new_type;
	elements.appearance_new_name_label3=appearance_new_name_label3;
	elements.appearance_new_create=appearance_new_create;

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
}

void appObj::appearances_initialize(ONLY IN_THREAD)
{
	appearance_info_t::lock lock{appearance_info};

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
	combobox_items.insert(combobox_items.end(),
			      lock->ids.begin(),
			      lock->ids.end());

	x::w::standard_comboboxlayoutmanager lm=
		appearance_name->get_layoutmanager();

	lm->replace_all_items(IN_THREAD, combobox_items);

	// The autoselect will initialize the rest.
	lm->autoselect(IN_THREAD, 0, {});
}

void appObj::appearance_selected
(ONLY IN_THREAD,
 const x::w::standard_combobox_selection_changed_info_t &info)
{
	if (!info.list_item_status_info.selected)
		return;

	appearance_info_t::lock lock{appearance_info};

	lock->current_selection.reset();
	size_t n=info.list_item_status_info.item_number;

	if (n == 0)
	{
		appearance_new_name_label->show(IN_THREAD);
		appearance_new_name->show(IN_THREAD);
		appearance_new_name_label2->show(IN_THREAD);
		appearance_new_type->show(IN_THREAD);
		appearance_new_name_label3->show(IN_THREAD);
		appearance_new_create->show(IN_THREAD);
	}
	else
	{
		appearance_new_name_label->hide(IN_THREAD);
		appearance_new_name->hide(IN_THREAD);
		appearance_new_name_label2->hide(IN_THREAD);
		appearance_new_type->hide(IN_THREAD);
		appearance_new_name_label3->hide(IN_THREAD);
		appearance_new_create->hide(IN_THREAD);

		lock->current_selection.emplace();

		auto &current_selection=
			*lock->current_selection;

		current_selection.index=n-1;
	}

	appearance_enable_disable_buttons(IN_THREAD, lock);
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

void appObj::appearance_enable_disable_buttons(ONLY IN_THREAD,
					       appearance_info_t::lock &lock)
{
	lock->new_appearance_name=
		x::w::input_lock{appearance_new_name}.get();

	x::w::standard_comboboxlayoutmanager appearance_type_lm=
		appearance_new_type->get_layoutmanager();

	auto appearance_type_selected=appearance_type_lm->selected();

	lock->new_appearance_type.clear();

	if (!lock->current_selection && appearance_type_selected)
	{
		std::vector<std::string> appearance_type_list;

		appearance_type_list.reserve(appearance_types.size());

		for (const auto &type:appearance_types)
			appearance_type_list.push_back(type.first);

		lock->new_appearance_type=appearance_type_list
			.at(*appearance_type_selected);
	}

	appearance_new_create->set_enabled(!lock->new_appearance_name.empty() &&
					   !lock->new_appearance_type.empty());
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

	if (lock->new_appearance_name.empty() ||
	    lock->new_appearance_type.empty())
		return ret;

	auto created=create_update("appearance",
				   lock->new_appearance_name, true);

	auto &[doc_lock, new_appearance]=*created;

	doc_lock->attribute({"type", lock->new_appearance_type});

	ret.emplace(doc_lock,
		    [=, saved_lock=lock.threadlock(x::ref{this})]
		    (appObj *me,
		     const x::ref<x::obj> &busy_mcguffin)
		    {
			    appearance_info_t::lock lock{saved_lock};

			    me->appearance_create2(lock,
						   busy_mcguffin);
		    });

	return ret;
}

void appObj::appearance_create2(appearance_info_t::lock &lock,
				const x::ref<x::obj> &busy_mcguffin)
{
	update_new_element(lock->new_appearance_name, lock->ids,
			   appearance_name);

	main_window->in_thread_idle
		([busy_mcguffin]
		 (ONLY IN_THREAD)
		 {
		 });
}
