#include "libcxxw_config.h"

#include "creator/app.H"
#include "x/w/uielements.H"
#include "x/w/standard_comboboxlayoutmanager.H"
#include "x/w/input_field_lock.H"
#include "messages.H"
#include <x/messages.H>
#include <x/xml/escape.H>
#include <cmath>
#include <set>

void appObj::dimension_elements_initialize(app_elements_tptr &elements,
					   x::w::uielements &ui,
					   init_args &args)
{
	x::w::focusable_container dimension_name=
		ui.get_element("dimension_name_field");
	x::w::label dimension_new_name_label=
		ui.get_element("dimension_new_name_label");
	x::w::input_field dimension_new_name=
		ui.get_element("dimension_new_name_field");
	x::w::image_button dimension_value_option=
		ui.get_element("dimension_value_option_field");
	x::w::input_field dimension_value=
		ui.get_element("dimension_value_field");
	x::w::image_button dimension_scale_option=
		ui.get_element("dimension_scale_option_field");
	x::w::focusable_container dimension_from_name=
		ui.get_element("dimension_from_name_field");
	x::w::input_field dimension_scale_value=
		ui.get_element("dimension_scale_value_field");
	x::w::button dimension_update_button=
		ui.get_element("dimension_update_button");
	x::w::button dimension_reset_button=
		ui.get_element("dimension_reset_button");
	x::w::button dimension_delete_button=
		ui.get_element("dimension_delete_button");

	x::w::standard_comboboxlayoutmanager dimension_name_lm=
		dimension_name->get_layoutmanager();

	dimension_name_lm->selection_changed
		([]
		 (ONLY IN_THREAD,
		  const auto &info)
		 {
			 appinvoke(&appObj::dimension_selected, IN_THREAD,
				   info);
		 });

	x::w::standard_comboboxlayoutmanager dimension_scaled_name_lm=
		dimension_from_name->get_layoutmanager();

	dimension_scaled_name_lm->selection_changed
		([]
		 (ONLY IN_THREAD,
		  const auto &info)
		 {
			 appinvoke(&appObj::dimension_from_selected, IN_THREAD,
				   info);
		 });

	dimension_value_option->on_activate
		([]
		 (ONLY IN_THREAD,
		  size_t i,
		  const auto &trigger,
		  const auto &busy)
		 {
			 if (i)
				 appinvoke(&appObj
					   ::dimension_value_option_selected,
					   IN_THREAD);
		 });

	dimension_scale_option->on_activate
		([]
		 (ONLY IN_THREAD,
		  size_t i,
		  const auto &trigger,
		  const auto &busy)
		 {
			 if (i)
				 appinvoke(&appObj
					   ::dimension_scale_option_selected,
					   IN_THREAD);
		 });

	dimension_new_name->on_filter(args.label_filter);

	dimension_update_button->on_activate
		([]
		 (ONLY IN_THREAD,
		  const auto &grigger,
		  const auto &busy)
		 {
			 appinvoke(&appObj::update_theme,
				   IN_THREAD, busy,
				   &appObj::dimension_validate,
				   &appObj::dimension_update);

		 });

	dimension_reset_button->on_activate
		([]
		 (ONLY IN_THREAD,
		  const auto &grigger,
		  const auto &busy)
		 {
			 appinvoke(&appObj::dimension_reset);
		 });

	dimension_delete_button->on_activate
		([]
		 (ONLY IN_THREAD,
		  const auto &grigger,
		  const auto &busy)
		 {
			 appinvoke(&appObj::update_theme,
				   IN_THREAD, busy,
				   &appObj::dimension_ok_to_delete,
				   &appObj::dimension_delete);
		 });

	elements.dimension_name=dimension_name;
	elements.dimension_new_name_label=dimension_new_name_label;
	elements.dimension_new_name=dimension_new_name;
	elements.dimension_value_option=dimension_value_option;
	elements.dimension_value=dimension_value;
	elements.dimension_scale_option=dimension_scale_option;
	elements.dimension_from_name=dimension_from_name;
	elements.dimension_scale_value=dimension_scale_value;
	elements.dimension_update_button=dimension_update_button;
	elements.dimension_reset_button=dimension_reset_button;
	elements.dimension_delete_button=dimension_delete_button;
}

// Create an xpath for a particular dim.
static auto get_xpath_for(const x::xml::doc::base::readlock &lock,
			  const std::string &id)
{
	return lock->get_xpath("/theme/dim[@id='" +
			       x::xml::escapestr(id, true) +
			       "']");
}

void appObj::dimension_initialize()
{
	dimension_info_t::lock lock{dimension_info};

	x::w::standard_comboboxlayoutmanager lm=
		dimension_name->get_layoutmanager();

	auto existing_dims=theme.get()->readlock();

	existing_dims->get_root();
	auto xpath=existing_dims->get_xpath("/theme/dim");
	auto n=xpath->count();

	lock->ids.clear();
	lock->ids.reserve(n);

	for (size_t i=0; i<n; ++i)
	{
		xpath->to_node(i+1);

		lock->ids.push_back(existing_dims->get_any_attribute("id"));
	}

	std::sort(lock->ids.begin(), lock->ids.end());

	// Duplicate IDS: TODO - report this.

	lock->ids.erase(std::unique(lock->ids.begin(), lock->ids.end()),
			lock->ids.end());

	std::vector<x::w::list_item_param> combobox_items;

	combobox_items.reserve(lock->ids.size()+1);

	combobox_items.push_back(_("-- New Dimension --"));
	combobox_items.insert(combobox_items.end(),
			      lock->ids.begin(),
			      lock->ids.end());
	lm->replace_all_items(combobox_items);

	// The same list goes into the from scale dropdown.

	combobox_items[0]=_("-- Scaled Dimension --");
	x::w::standard_comboboxlayoutmanager from_lm=
		dimension_from_name->get_layoutmanager();
	from_lm->replace_all_items(combobox_items);

	// Can only do this after both combo-boxes are initializes, due
	// to the selection callback that expects everything to be there.
	lm->autoselect(0);
}

void appObj::dimension_selected(ONLY IN_THREAD,
				const
				x::w::standard_combobox_selection_changed_info_t
				&info)
{
	dimension_info_t::lock lock{dimension_info};

	size_t n=info.list_item_status_info.item_number;

	if (info.list_item_status_info.selected)
	{
		x::w::standard_comboboxlayoutmanager
			lm=dimension_from_name->get_layoutmanager();

		if (n == 0) // New dimension entry
		{
			lock->current_selection.reset();
			dimension_new_name_label->show(IN_THREAD);
			dimension_new_name->show(IN_THREAD);
			dimension_delete_button->set_enabled(false);
			lm->autoselect(0);
		}
		else
		{
			lock->current_selection.emplace();
			auto &orig_params=*lock->current_selection;
			orig_params.index=--n;

			auto current_value=theme.get()->readlock();
			current_value->get_root();

			auto xpath=get_xpath_for(current_value,
						 n < lock->ids.size()
						 ? lock->ids[n]
						 : "" /* Boom, on next line */);

			// We expect one to be there, of course.
			xpath->to_node(1);

			auto scale_from=
				current_value->get_any_attribute("scale");

			if (!scale_from.empty())
			{
				auto pos=std::lower_bound(lock->ids.begin(),
							  lock->ids.end(),
							  scale_from);
				if (pos != lock->ids.end() &&
				    *pos == scale_from)
				{
					orig_params.scale_from=
						pos-lock->ids.begin();
				}
			}
			orig_params.value=current_value->get_text();

			dimension_new_name_label->hide(IN_THREAD);
			dimension_new_name->hide(IN_THREAD);
			dimension_delete_button->set_enabled(true);

			// Disable the corresponding item in the from scale
			// combo-box.
			lm->enabled(n, false);
		}
		dimension_reset_values(lock);
	}
	else
	{
		// When a selection is changed, the old value is always
		// deselected first. So, hook this to clear and reset all
		// fields.

		dimension_new_name->set("");
		dimension_value->set("");
		dimension_scale_value->set("");

		x::w::standard_comboboxlayoutmanager
			lm=dimension_from_name->get_layoutmanager();

		// Reenable this one.

		lm->enabled(IN_THREAD, n, true);
	}
}

void appObj::dimension_reset()
{
	dimension_info_t::lock lock{dimension_info};

	dimension_reset_values(lock);

	if (lock->current_selection)
	{
		if (lock->current_selection->scale_from)
		{
			dimension_scale_value->request_focus();
		}
		else
		{
			dimension_value->request_focus();
		}
	}
	else
	{
		dimension_new_name->request_focus();
	}
}

bool appObj::dimension_unsaved_values(dimension_info_t::lock &lock)
{
	if (!lock->current_selection)
		return lock->save_params ? true:false; // New selection

	if (lock->save_params && *lock->current_selection == *lock->save_params)
		return false;

	return true;
}

void appObj::dimension_enable_disable_buttons(dimension_info_t::lock &lock)
{
	bool unchanged=false;

	if (dimension_unsaved_values(lock))
	{
		dimension_update_button->set_enabled(true);
		dimension_reset_button->set_enabled(true);
	}
	else
	{
		if (lock->current_selection)
		{
			// No changes

			dimension_update_button->set_enabled(false);
			dimension_reset_button->set_enabled(false);
			unchanged=true;
		}
		else
		{
			// New dimension. Values not valid, so no update,
			// but a reset is available.

			dimension_update_button->set_enabled(false);
			dimension_reset_button->set_enabled(true);
		}
	}

	dimension_delete_button->set_enabled(unchanged);
}

void appObj::dimension_reset_values(dimension_info_t::lock &lock)
{
	x::w::standard_comboboxlayoutmanager
		lm=dimension_from_name->get_layoutmanager();

	if (!lock->current_selection)
	{
		dimension_value_option->set_value(1);
		dimension_new_name->set("");
		dimension_value->set("");
		dimension_scale_value->set("");
		lm->autoselect(0);
	}
	else
	{
		auto &orig_values=*lock->current_selection;

		if (orig_values.scale_from)
		{
			dimension_scale_option->set_value(1);
			lm->autoselect(*orig_values.scale_from+1);
			dimension_scale_value->set(orig_values.value);
			dimension_value->set("");
		}
		else
		{
			dimension_value_option->set_value(1);
			dimension_value->set(orig_values.value);
			lm->autoselect(0);
			dimension_scale_value->set("");
		}
	}

	// After all of the above are processed by the connection thread,
	// revalidate all fields.
	main_window->in_thread
		([]
		 (ONLY IN_THREAD)
		 {
			 appinvoke([&]
				   (appObj *me)
				   {
					   me->dimension_value
						   ->validate_modified
						   (IN_THREAD);
					   me->dimension_scale_value
						   ->validate_modified
						   (IN_THREAD);
				   });
		 });
}

void appObj
::dimension_from_selected(ONLY IN_THREAD,
			  const
			  x::w::standard_combobox_selection_changed_info_t
			  &info)
{
	dimension_info_t::lock lock{dimension_info};

	size_t n=info.list_item_status_info.item_number;

	if (info.list_item_status_info.selected)
	{
		lock->from_index.reset();

		if (n > 0)
		{
			lock->from_index=n-1;
			dimension_scale_option->set_value(IN_THREAD, 1);
			dimension_scale_option_selected(IN_THREAD);
		}
		dimension_field_updated(IN_THREAD);
	}
}

// Dimension values are validated as std::strings. Check if the dimension
// value is validated as a non-empty string.

static bool validated_non_empty(x::mpobj<std::optional<std::string>> &v)
{
	x::mpobj<std::optional<std::string>>::lock lock{v};

	return *lock && (*lock)->size() > 0;
}

void appObj::dimension_value_entered(ONLY IN_THREAD)
{
	if (validated_non_empty(dimension_value_validated->validated_value))
		// Set the radio button to ourselves
		dimension_value_option->set_value(IN_THREAD, 1);

	dimension_field_updated(IN_THREAD);
}

void appObj::dimension_scale_value_entered(ONLY IN_THREAD)
{
	if (validated_non_empty(dimension_scale_value_validated
				->validated_value))
		// If a valid value was entered, set the radio button
		// to ourselves.
		dimension_scale_option->set_value(IN_THREAD, 1);

	dimension_field_updated(IN_THREAD);
}

void appObj::dimension_value_option_selected(ONLY IN_THREAD)
{
	// Clear the scale values, the value option is now selected.

	x::w::standard_comboboxlayoutmanager lm=
		dimension_from_name->get_layoutmanager();

	lm->unselect(IN_THREAD);
	dimension_scale_value->set("");
}

void appObj::dimension_scale_option_selected(ONLY IN_THREAD)
{
	// Clear the explicit value, the scale option is selected.

	dimension_value->set("");
}

bool appObj::dimension_validate(ONLY IN_THREAD)
{
	return dimension_new_name->validate_modified(IN_THREAD) &&
		dimension_value->validate_modified(IN_THREAD) &&
		dimension_scale_value->validate_modified(IN_THREAD);
}

void appObj::dimension_field_updated(ONLY IN_THREAD)
{
	dimension_info_t::lock lock{dimension_info};

	bool value=dimension_update_save_params(IN_THREAD, lock);

	if (!value)
		lock->save_params.reset();

	dimension_enable_disable_buttons(lock);
}

bool appObj::dimension_update_save_params(ONLY IN_THREAD,
					  dimension_info_t::lock &lock)
{
	lock->save_params.emplace();

	auto &new_params=*lock->save_params;

	if (!lock->current_selection)
	{
		auto new_name=
			dimension_new_name_validated->validated_value.get();

		if (!new_name || !new_name)
		{
			return false;
		}

		new_params.dimension_new_name=*new_name;
	}

	if (dimension_value_option->get_value())
	{
		// Check if a good validated value exists

		auto value=dimension_value_validated->validated_value.get();

		if (value && value->size() > 0)
		{
			new_params.value=*value;
		}
		else
		{
			return false;
		}
	}
	else
	{
		// Check if the from value is set.
		if (lock->from_index && *lock->from_index <
		    lock->ids.size())
		{
			new_params.scale_from= *lock->from_index;
		}
		else
		{
			return false; // Shouldn't happen.
		}

		// Check if the scaled value is set

		auto value=
			dimension_scale_value_validated->validated_value.get();

		if (value && value->size())
		{
			new_params.value=*value;
		}
		else
		{
			return false;
		}
	}

	return true;
}

// Helper for creating a new <dim>
//
// If there are existing dims, create the new dim just before the first one.
//
// Otherwise create one as the first element, lock is positioned at /theme.

static inline auto create_new_dim(const x::xml::doc::base::writelock &lock,
				  const x::xml::doc::base::xpath &existing_dims,
				  const std::string &name)
{
	if (existing_dims->count() > 0)
	{
		existing_dims->to_node(1);
		return lock->create_previous_sibling();
	}

	return lock->create_child();
}

void appObj::dimension_update(const update_callback_t &callback)
{
	dimension_info_t::lock lock{dimension_info};

	if (!lock->save_params)
		return;

	auto &save_params=*lock->save_params;

	auto new_doc=theme.get()->readlock()->clone_document();

	auto doc_lock=new_doc->writelock();

	doc_lock->get_root();

	std::string id=save_params.dimension_new_name;
	bool is_new=true;

	if (lock->current_selection)
	{
		id=lock->ids.at(lock->current_selection->index);
		is_new=false;
	}

	auto xpath=get_xpath_for(doc_lock, id);

	// This one already exists?

	if (xpath->count() > 0)
	{
		if (is_new) // It shouldn't
		{
			std::string error=
				x::gettextmsg(_("Dimension %1% "
						"already exists"), id);
			main_window->stop_message(error);

			return;
		}

		xpath->to_node(1); // Remove existing dim
		doc_lock->remove();
	}

	doc_lock->get_xpath("/theme")->to_node();
	xpath=doc_lock->get_xpath("dim");

	auto new_dim=create_new_dim(doc_lock,
				    xpath,
				    save_params.dimension_new_name);

	new_dim=new_dim->element({"dim"})->create_child()
		->attribute({"id", id});

	if (save_params.value == "inf")
		new_dim->text("inf");
	else
	{
		if (save_params.scale_from)
			new_dim=new_dim->attribute
				({"scale",
				  lock->ids.at(*save_params.scale_from)});

		new_dim->text(save_params.value);
	}

	if (!callback(doc_lock->clone_document()))
		return;

	// Update accepted.

	if (is_new)
	{
		// Move the focus here first.
		dimension_name->request_focus();

		auto insert_pos=std::lower_bound(lock->ids.begin(),
						 lock->ids.end(),
						 id);

		x::w::standard_comboboxlayoutmanager name_lm=
			dimension_name->get_layoutmanager(),
			from_name_lm=dimension_from_name->get_layoutmanager();

		auto i=insert_pos-lock->ids.begin()+1;
		// Pos 0 is new dimension

		lock->ids.insert(insert_pos, id);
		name_lm->insert_items(i, {id});

		// Insert the new value in the from scale combo-box too.
		from_name_lm=dimension_from_name->get_layoutmanager();
		from_name_lm->insert_items(i, {id});

		name_lm->autoselect(i);
		status->update(_("Created new dimension"));
	}
	else
	{
		dimension_reset_values(lock);
		status->update(_("Dimension updated"));
	}
}

bool appObj::dimension_ok_to_delete(ONLY IN_THREAD)
{
	return true; // This is validated by the enabled status
}

void appObj::dimension_delete(const update_callback_t &callback)
{
	dimension_info_t::lock lock{dimension_info};

	if (!lock->current_selection)
		return;

	auto index=lock->current_selection->index;
	auto id=lock->ids.at(index);

	auto new_doc=theme.get()->readlock()->clone_document();

	auto doc_lock=new_doc->writelock();

	while (1)
	{
		doc_lock->get_root();

		auto xpath=get_xpath_for(doc_lock, id);

		if (xpath->count() <= 0)
			break;

		xpath->to_node(1);
		doc_lock->remove();
	}

	if (!callback(doc_lock->clone_document()))
		return;

	lock->ids.erase(lock->ids.begin()+index);

	x::w::standard_comboboxlayoutmanager name_lm=
		dimension_name->get_layoutmanager(),
		from_name_lm=dimension_from_name->get_layoutmanager();

	name_lm->remove_item(index+1);
	from_name_lm->remove_item(index+1);

	lock->current_selection.reset();
	dimension_reset_values(lock);
	name_lm->autoselect(0);

	// autoselect(0) queues up a request.
	// When it gets processed, the dimension new name field gets show()n.
	// We want to request focus for that after all that processing also
	// gets done:

	main_window->in_thread_idle
		([]
		 (ONLY IN_THREAD)
		 {
			 appinvoke([]
				   (appObj *me)
				   {
					   me->dimension_new_name
						   ->request_focus();
				   });
		 });
	status->update(_("Deleted"));
}
