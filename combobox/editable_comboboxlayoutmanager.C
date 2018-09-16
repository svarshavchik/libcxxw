/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "combobox/editable_comboboxlayoutmanager.H"
#include "input_field.H"
#include "editor_impl.H"
#include "x/w/input_field_lock.H"
#include "x/w/impl/container.H"
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

static inline bool autocomplete(ONLY IN_THREAD,
				const container_impl &combobox_container_impl,
				input_autocomplete_info_t &autocomplete_info)
{
	layout_implptr layoutmanager_impl;

	combobox_container_impl->invoke_layoutmanager
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
			// The initial{} callback trigger is referenced in the
			// selection_changed callback, below.
			lm->autoselect(IN_THREAD, found, initial{});
		return true;
	}
	lm->unselect();
	return false;
}

new_editable_comboboxlayoutmanager
::new_editable_comboboxlayoutmanager(const editable_combobox_selection_changed_t
				     &selection_changed)
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

		   // We have our own border, no need for the input field
		   // do add its own.
		   config.border="empty";

		   auto input_field=f->create_input_field
			   ({theme_font({f->get_element_impl()
							   .label_theme_font()
							   })
					   }, config);

		   input_field->on_autocomplete
			   ([container_impl=make_weak_capture(f->get_container_impl())]
			    (ONLY IN_THREAD,
			     auto &autocomplete_info) {

				   bool flag=false;

				   auto got=container_impl.get();

				   if (got)
				   {
					   auto & [ci]=*got;
					   flag=autocomplete(IN_THREAD,
							     ci,
							     autocomplete_info);
				   }
				   return flag;
			   });

		   return input_field;
	   }),
	  selection_changed{ selection_changed }
{
}

static editable_combobox_selection_changed_t noop_selection_changed=
	[]
	(THREAD_CALLBACK, const auto &ignore)
{
};

new_editable_comboboxlayoutmanager::new_editable_comboboxlayoutmanager()
	: new_editable_comboboxlayoutmanager{noop_selection_changed}
{
}

new_editable_comboboxlayoutmanager::~new_editable_comboboxlayoutmanager()
=default;

static custom_combobox_selection_changed_t editable_selection_changed=
	[]
	(ONLY IN_THREAD, const auto &info)
	{
		editable_comboboxlayoutmanager lm=info.lm;
		x::w::input_field current_selection=info.current_selection;

		standard_combobox_lock lock{lm};

		info.popup_element->impl->request_visibility(IN_THREAD, false);

		auto editor_impl=current_selection->impl->editor_element->impl;

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

				editor_impl->set(IN_THREAD,
						 lock.item
						 (info.list_item_status_info
						  .item_number).string);

				// Make sure that any validation callback
				// that gets installed into the input element
				// will process the new item. We do this by
				// setting the required flag, moving the input
				// focus into the editor, and invoking the
				// validation function.
				editor_impl->validation_required(IN_THREAD)=
					true;

				focusableObj &f=*current_selection;

				f.get_impl()->request_focus(IN_THREAD);
				editor_impl->validate_modified(IN_THREAD, {});
			}
		}
		// else - don't clear the input field
		//
		// The input field in an editable combo-box is free-form
		// association, and just a suggestion. Besides, if the
		// input field has a validator, it won't know about this.

		// The busy mcguffin in info is the busy
		// mcguffin for the popup window. The callback
		// would probably want to install the busy
		// mcguffin for the window that
		// contains the combo-box.
		busy_impl yes_i_am{*current_selection->elementObj::impl};

		lm->impl->selection_changed.get()
		(IN_THREAD, editable_combobox_selection_changed_info_t{
			lm, lock, info.list_item_status_info, yes_i_am, lm});
	};


custom_combobox_selection_changed_t new_editable_comboboxlayoutmanager
::get_selection_changed() const
{
	return editable_selection_changed;
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
