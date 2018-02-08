/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "combobox/editable_comboboxlayoutmanager.H"
#include "x/w/input_field.H"
#include "x/w/input_field_lock.H"
#include "container.H"
#include "busy.H"
#include <x/weakcapture.H>

LIBCXXW_NAMESPACE_START

editable_comboboxlayoutmanagerObj
::editable_comboboxlayoutmanagerObj(const ref<implObj> &impl,
				    const ref<listlayoutmanagerObj::implObj>
				    &list_layout_impl)
	: standard_comboboxlayoutmanagerObj(impl, list_layout_impl),
	  impl(impl)
{
}

editable_comboboxlayoutmanagerObj::~editable_comboboxlayoutmanagerObj()=default;

input_lock::input_lock(const editable_comboboxlayoutmanagerObj &e)
	: input_lock(const_input_field(e.current_selection()))
{
}

///////////////////////////////////////////////////////////////////////////

static inline bool autocomplete(const auto &container_impl,
				auto &autocomplete_info)
{
	ptr<layoutmanagerObj::implObj> layoutmanager_impl;

	container_impl->invoke_layoutmanager
		([&]
		 (const auto &lm_impl)
		 {
			 layoutmanager_impl=lm_impl;
		 });

	if (!layoutmanager_impl)
		return false;

	editable_comboboxlayoutmanager
		lm=layoutmanager_impl->create_public_object();

	standard_combobox_lock lock{lm};

	size_t found;

	if (lock.search(0, autocomplete_info.string, found, true))
	{
		autocomplete_info.selection_start=
			autocomplete_info.string.size();
		autocomplete_info.string=lock.item(found).string;

		if (!lm->selected(found))
			lm->autoselect(found, initial{});
		return true;
	}
	lm->unselect();
	return false;
}

new_editable_comboboxlayoutmanager
::new_editable_comboboxlayoutmanager()
	: new_custom_comboboxlayoutmanager
	  ([]
	   (const auto &f)
	   {
		   // Make sure input field's default font matches the
		   // labels'.

		   // The field gets automatically sized by the combobox
		   // layout manager accoridng to the width of the combobox
		   // items, so for the purpose of creating the input field
		   // make it think it's only two columns wide;

		   input_field_config config{2,
				   1, // One row,
				   true,	// autoselect
				   true,	// autodeselect
				   false	// do not update clipboards
				   };

		   auto input_field=f->create_input_field
			   ({theme_font({f->get_element_impl()
							   .label_theme_font()
							   })
					   }, config);

		   input_field->on_autocomplete
			   ([container_impl=make_weak_capture(f->get_container_impl())]
			    (auto &autocomplete_info) {

				   bool flag=false;

				   auto got=container_impl.get();

				   if (got)
				   {
					   auto & [ci]=*got;
					   flag=autocomplete(ci,
							     autocomplete_info);
				   }
				   return flag;
			   });

		   return input_field;
	   }),
	  selection_changed{ [](const auto &ignore) {}}
{
}

new_editable_comboboxlayoutmanager
::new_editable_comboboxlayoutmanager(const editable_combobox_selection_changed_t
				     &selection_changed)
	: new_editable_comboboxlayoutmanager()
{
	this->selection_changed=selection_changed;
}

new_editable_comboboxlayoutmanager::~new_editable_comboboxlayoutmanager()
=default;

custom_combobox_selection_changed_t new_editable_comboboxlayoutmanager
::get_selection_changed() const
{
	return []
		(const auto &info)
	{
		editable_comboboxlayoutmanager lm=info.lm;
		x::w::input_field current_selection=info.current_selection;

		standard_combobox_lock lock{lm};

		info.popup_element->hide();
		if (info.list_item_status_info.selected)
		{
			// initial trigger variant is specified when the
			// combo-box gets selected in autocomplete(). Don't
			// overwrite the input field, autocomplete() will
			// take care of it for us.

			if (!std::holds_alternative<initial>
			    (info.list_item_status_info.trigger))
			{
				input_lock i_lock{current_selection};

				current_selection
					->set(lock.item
					      (info.list_item_status_info
					       .item_number).string);
			}
		}
		else // Unselected.
		{
			input_lock i_lock{current_selection};

			if (i_lock.get_unicode() ==
			    lock.item(info.list_item_status_info.item_number)
			    .string)
			{
				current_selection->set("");
			}
		}

		// The busy mcguffin in info is the busy
		// mcguffin for the popup window. The callback
		// would probably want to install the busy
		// mcguffin for the window that
		// contains the combo-box.
		busy_impl yes_i_am{*current_selection->elementObj::impl};

		lm->impl->selection_changed.get()
			({lock, info.list_item_status_info, yes_i_am, lm});
	};
}

void editable_comboboxlayoutmanagerObj
::selection_changed(const editable_combobox_selection_changed_t &cb)
{
	impl->selection_changed=cb;
}

ref<custom_comboboxlayoutmanagerObj::implObj>
new_editable_comboboxlayoutmanager
::create_impl(const create_impl_info &i) const
{
	return ref<editable_comboboxlayoutmanagerObj::implObj>
		::create(i.container_impl, *this);
}

LIBCXXW_NAMESPACE_END
