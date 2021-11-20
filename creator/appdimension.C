#include "libcxxw_config.h"

#include "creator/app.H"
#include "x/w/uielements.H"
#include "x/w/standard_comboboxlayoutmanager.H"
#include "x/w/editable_comboboxlayoutmanager.H"
#include "x/w/input_field_lock.H"
#include "x/w/impl/uixmlparser.H"
#include "messages.H"
#include <x/messages.H>
#include <x/mpthreadlock.H>
#include <cmath>
#include <set>

// Other pages' static widgets that enumerate available dimensions

namespace {
#if 0
}
#endif

struct other_dimension_widget {
	x::w::focusable_container appObj::*widget;
};

static const other_dimension_widget other_dimension_widgets[]=
	{
	 { &appObj::border_width },
	 { &appObj::border_height },
	 { &appObj::border_hradius },
	 { &appObj::border_vradius },
	};
#if 0
{
#endif
}

void appObj::dimension_elements_create(x::w::uielements &ui)
{
	// Validator for the dimension_value_field
	create_value_validator(
		ui,
		"dimension_value_field",
		true,
		_txt("Invalid millimeter value"),
		&appObj::dimension_value_entered
	);

	// Validator for the dimension_scale_value_field
	create_value_validator(
		ui,
		"dimension_scale_value_field",
		false,
		_txt("Invalid scale value"),
		&appObj::dimension_scale_value_entered
	);

	// Validator for the new name field
	ui.create_validated_input_field(
		"dimension_new_name_field",
		[]
		(ONLY IN_THREAD,
		 const std::string &value,
		 const auto &lock,
		 const auto &trigger)
		-> std::optional<std::string>
		{
			return value;
		},
		[]
		(const std::string &v) -> std::string
		{
			return v;
		},
		std::nullopt,
		[]
		(ONLY IN_THREAD,
		 const std::optional<std::string> &v)
		{
			appinvoke(&appObj::dimension_field_updated, IN_THREAD);
		}
	);
}

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
	auto dimension_new_name_validated=
		ui.get_validated_input_field<std::string>(
			"dimension_new_name_field"
		);
	x::w::image_button dimension_value_option=
		ui.get_element("dimension_value_option_field");
	x::w::input_field dimension_value=
		ui.get_element("dimension_value_field");
	auto dimension_value_validated=
		ui.get_validated_input_field<std::string>(
			"dimension_value_field"
		);
	x::w::image_button dimension_scale_option=
		ui.get_element("dimension_scale_option_field");
	x::w::focusable_container dimension_from_name=
		ui.get_element("dimension_from_name_field");
	x::w::input_field dimension_scale_value=
		ui.get_element("dimension_scale_value_field");
	auto dimension_scale_value_validated=
		ui.get_validated_input_field<std::string>(
			"dimension_scale_value_field"
		);
	x::w::button dimension_update_button=
		ui.get_element("dimension_update_button");
	x::w::button dimension_reset_button=
		ui.get_element("dimension_reset_button");
	x::w::button dimension_delete_button=
		ui.get_element("dimension_delete_button");

	x::w::standard_comboboxlayoutmanager dimension_name_lm=
		dimension_name->get_layoutmanager();

	dimension_name_lm->on_selection_changed
		([]
		 (ONLY IN_THREAD,
		  const auto &info)
		 {
			 appinvoke(&appObj::dimension_selected, IN_THREAD,
				   info);
		 });

	x::w::standard_comboboxlayoutmanager dimension_scaled_name_lm=
		dimension_from_name->get_layoutmanager();

	dimension_scaled_name_lm->on_selection_changed
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

	dimension_new_name->on_filter(get_label_filter());

	dimension_update_button->on_activate
		([]
		 (ONLY IN_THREAD,
		  const auto &grigger,
		  const auto &busy)
		 {
			 appinvoke(&appObj::update_theme,
				   IN_THREAD, busy,
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
				   &appObj::dimension_delete);
		 });

	elements.dimension_name=dimension_name;
	elements.dimension_new_name_label=dimension_new_name_label;
	elements.dimension_new_name=dimension_new_name;
	elements.dimension_new_name_validated=dimension_new_name_validated;
	elements.dimension_value_option=dimension_value_option;
	elements.dimension_value=dimension_value;
	elements.dimension_value_validated=dimension_value_validated;
	elements.dimension_scale_option=dimension_scale_option;
	elements.dimension_from_name=dimension_from_name;
	elements.dimension_scale_value=dimension_scale_value;
	elements.dimension_scale_value_validated=
		dimension_scale_value_validated;
	elements.dimension_update_button=dimension_update_button;
	elements.dimension_reset_button=dimension_reset_button;
	elements.dimension_delete_button=dimension_delete_button;
}

void appObj::dimension_initialize(ONLY IN_THREAD)
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
	lm->replace_all_items(IN_THREAD, combobox_items);

	// The same list goes into the from scale dropdown.

	combobox_items[0]=_("-- Scaled Dimension --");
	x::w::standard_comboboxlayoutmanager from_lm=
		dimension_from_name->get_layoutmanager();
	from_lm->replace_all_items(IN_THREAD, combobox_items);

	// Can only do this after both combo-boxes are initialized, due
	// to the selection callback that expects everything to be there.
	lm->autoselect(IN_THREAD, 0, {});

	// Initialize widgets on other pages.
	combobox_items.erase(combobox_items.begin());
	for (const auto &other:other_dimension_widgets)
	{
		x::w::editable_comboboxlayoutmanager lm=
			(this->*(other.widget))->get_layoutmanager();
		lm->replace_all_items(combobox_items);
	}
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

			// Disable the corresponding item in the from scale
			// combo-box.
			//
			// Not IN THREAD because we can be here after creating
			// a new dimension, and from_name's combo-box has not
			// been updated yet.
			lm->enabled(n, false);

			lock->current_selection.emplace();
			auto &orig_params=*lock->current_selection;
			orig_params.index=--n;

			auto current_value=theme.get()->readlock();
			current_value->get_root();

			auto xpath=get_xpath_for(current_value,
						 "dim",
						 n < lock->ids.size()
						 ? lock->ids[n]
						 : "" /* Boom, on next line */);

			// We expect one to be there, of course.
			xpath->to_node(1);

			auto d=x::w::ui::parse_dim(current_value);

			if (!d.scale.empty())
			{
				auto pos=std::lower_bound(lock->ids.begin(),
							  lock->ids.end(),
							  d.scale);
				if (pos != lock->ids.end() &&
				    *pos == d.scale)
				{
					orig_params.scale_from=
						pos-lock->ids.begin();
				}
			}
			orig_params.value=d.value;

			dimension_new_name_label->hide(IN_THREAD);
			dimension_new_name->hide(IN_THREAD);
			dimension_delete_button->set_enabled(true);
		}
		dimension_reset_values(lock);
	}
	else
	{
		// When a selection is changed, the old value is always
		// deselected first. So, hook this to clear and reset all
		// fields.

		dimension_new_name_validated->set(IN_THREAD, std::nullopt);
		dimension_value_validated->set(IN_THREAD, std::nullopt);
		dimension_scale_value_validated->set(IN_THREAD, std::nullopt);

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

void appObj::dimension_enable_disable_buttons(dimension_info_t::lock &lock)
{
	enable_disable_urd(lock,
			   dimension_update_button,
			   dimension_delete_button,
			   dimension_reset_button);
}

void appObj::dimension_reset_values(dimension_info_t::lock &lock)
{
	x::w::standard_comboboxlayoutmanager
		lm=dimension_from_name->get_layoutmanager();

	if (!lock->current_selection)
	{
		dimension_value_option->set_value(1);
		dimension_new_name_validated->set(std::nullopt);
		dimension_value_validated->set(std::nullopt);
		dimension_scale_value_validated->set(std::nullopt);
		lm->autoselect(0);
	}
	else
	{
		auto &orig_values=*lock->current_selection;

		if (orig_values.scale_from)
		{
			dimension_scale_option->set_value(1);
			lm->autoselect(*orig_values.scale_from+1);
			dimension_scale_value_validated->set(orig_values.value);
			dimension_value_validated->set(std::nullopt);
		}
		else
		{
			dimension_value_option->set_value(1);
			dimension_value_validated->set(orig_values.value);
			lm->autoselect(0);
			dimension_scale_value_validated->set(std::nullopt);
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

static bool validated_non_empty(const std::optional<std::string> &v)
{
	return v && v->size() > 0;
}

void appObj::dimension_value_entered(ONLY IN_THREAD)
{
	if (dimension_value_validated->access(validated_non_empty))
		// Set the radio button to ourselves
		dimension_value_option->set_value(IN_THREAD, 1);

	dimension_field_updated(IN_THREAD);
}

void appObj::dimension_scale_value_entered(ONLY IN_THREAD)
{
	if (dimension_scale_value_validated->access(validated_non_empty))
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
	dimension_scale_value_validated->set(IN_THREAD, std::nullopt);
}

void appObj::dimension_scale_option_selected(ONLY IN_THREAD)
{
	// Clear the explicit value, the scale option is selected.

	dimension_value_validated->set(IN_THREAD, std::nullopt);
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
		auto new_name=dimension_new_name_validated->value();

		if (!new_name || !new_name)
		{
			return false;
		}

		new_params.dimension_new_name=*new_name;
	}

	if (dimension_value_option->get_value())
	{
		// Check if a good validated value exists

		auto value=dimension_value_validated->value();

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

		auto value=dimension_scale_value_validated->value();

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

appObj::get_updatecallbackptr appObj::dimension_update(ONLY IN_THREAD)
{
	dimension_info_t::lock lock{dimension_info};

	bool validated=dimension_new_name->validate_modified(IN_THREAD) &&
		dimension_value->validate_modified(IN_THREAD) &&
		dimension_scale_value->validate_modified(IN_THREAD);

	if (!validated)
		return nullptr;

	return [validated, saved_lock=lock.threadlock(x::ref{this})]
		(appObj *me)
	       {
		       dimension_info_t::lock lock{saved_lock};

		       return me->dimension_update2(lock);
	       };
}

appObj::update_callback_t appObj::dimension_update2(dimension_info_t::lock
						     &lock)
{
	appObj::update_callback_t ret;

	if (!lock->save_params)
		return ret;

	auto &save_params=*lock->save_params;

	std::string id=save_params.dimension_new_name;
	bool is_new=true;

	if (lock->current_selection)
	{
		id=lock->ids.at(lock->current_selection->index);
		is_new=false;
	}

	auto created_update=create_update("dim", id, is_new);

	if (!created_update)
		return ret;

	auto &[doc_lock, new_dim]=*created_update;

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

	ret.emplace(doc_lock,
		    [=, saved_lock=lock.threadlock(x::ref{this})]
		    (appObj *me,
		     ONLY IN_THREAD,
		     const x::ref<x::obj> &busy_mcguffin)
		    {
			    dimension_info_t::lock lock{saved_lock};

			    me->dimension_update2(IN_THREAD,
						  lock, id, is_new,
						  busy_mcguffin);
		    });

	return ret;
}

void appObj::dimension_update2(ONLY IN_THREAD,
			       dimension_info_t::lock &lock,
			       const std::string &id,
			       bool is_new,
			       const x::ref<x::obj> &busy_mcguffin)
{
	// Update accepted.

	if (is_new)
	{
		// Insert the new value in the from scale combo-box too.
		x::w::standard_comboboxlayoutmanager
			from_name_lm=dimension_from_name->get_layoutmanager();


		auto i=update_new_element(IN_THREAD,
					  {id}, lock->ids,
					  dimension_name,
					  [&]
					  (size_t i)
					  {
						  from_name_lm->insert_items
							  (IN_THREAD, i, {id});
					  });

		if (lock->from_index && *lock->from_index >= i-1)
			++*lock->from_index;

		std::vector<x::w::list_item_param>
			new_item{ {lock->ids.at(i-1)}};

		for (const auto &other:other_dimension_widgets)
		{
			x::w::standard_comboboxlayoutmanager lm=
				(this->*(other.widget))->get_layoutmanager();

			lm->insert_items(IN_THREAD, i-1, new_item );
		}

		status->update(_("Created new dimension"));
	}
	else
	{
		dimension_reset_values(lock);
		status->update(_("Dimension updated"));
	}
}

appObj::get_updatecallbackptr appObj::dimension_delete(ONLY IN_THREAD)
{
	dimension_info_t::lock lock{dimension_info};

	return [saved_lock=lock.threadlock(x::ref{this})]
		(appObj *me)
	       {
		       dimension_info_t::lock lock{saved_lock};

		       return me->dimension_delete2(lock);
	       };
}

appObj::update_callback_t appObj::dimension_delete2(dimension_info_t::lock
						     &lock)
{
	appObj::update_callback_t ret;

	if (!lock->current_selection)
		return ret;

	auto index=lock->current_selection->index;
	auto id=lock->ids.at(index);

	auto new_doc=theme.get()->readlock()->clone_document();

	auto doc_lock=new_doc->writelock();

	while (1)
	{
		doc_lock->get_root();

		auto xpath=get_xpath_for(doc_lock, "dim", id);

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
			    dimension_info_t::lock lock{saved_lock};

			    me->dimension_delete3(IN_THREAD,
						  lock, index,
						  busy_mcguffin);
		    });

	return ret;
}

void appObj::dimension_delete3(ONLY IN_THREAD,
			       dimension_info_t::lock &lock,
			       size_t index,
			       const x::ref<x::obj> &busy_mcguffin)
{
	lock->ids.erase(lock->ids.begin()+index);

	x::w::standard_comboboxlayoutmanager name_lm=
		dimension_name->get_layoutmanager(),
		from_name_lm=dimension_from_name->get_layoutmanager();

	name_lm->remove_item(IN_THREAD, index+1);
	from_name_lm->remove_item(IN_THREAD, index+1);

	lock->current_selection.reset();
	dimension_reset_values(lock);
	name_lm->autoselect(IN_THREAD, 0, {});

	for (const auto &other:other_dimension_widgets)
	{
		name_lm=(this->*(other.widget))->get_layoutmanager();

		name_lm->remove_item(IN_THREAD, index);
	}

	dimension_new_name->request_focus(IN_THREAD);
	status->update(_("Deleted"));
}
