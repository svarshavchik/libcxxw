/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "combobox/editable_comboboxlayoutmanager.H"
#include "x/w/input_field.H"
#include "x/w/input_field_lock.H"
#include "container.H"
#include <x/weakcapture.H>

#include "listlayoutmanager/listlayoutmanager.H"

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

static inline bool autocomplete(auto &layoutmanager_impl,
				auto &autocomplete_info)
{
	editable_comboboxlayoutmanager
		lm=layoutmanager_impl->create_public_object();

	standard_combobox_lock lock{lm};

	size_t found;

	if (lock.search(0, autocomplete_info.string, found, true))
	{
		autocomplete_info.selection_start=
			autocomplete_info.string.size();
		autocomplete_info.string=lock.item(found).string;

		if (!lm->selected(lock, found))
			lm->autoselect(lock, found);
		return true;
	}
	lm->unselect();
	return false;
}

new_editable_comboboxlayoutmanager
::new_editable_comboboxlayoutmanager()
	: new_custom_comboboxlayoutmanager
	  ([]
	   (const auto &f,
	    const auto &popup_layoutmanager)
	   {
		   // Make sure input field's default font matches the
		   // labels'.

		   auto input_field=f->create_input_field
			   ({theme_font({popup_layoutmanager->
							   container_impl
							   ->get_element_impl()
							   .label_theme_font()
							   })
					   }, {2});

		   input_field->on_autocomplete
			   ([popup_layoutmanager=make_weak_capture(popup_layoutmanager)]
			    (auto &autocomplete_info) {

				   bool flag=false;

				   popup_layoutmanager
					   .get([&]
						(const auto &lm)
						{
							flag=autocomplete
								(lm,
								 autocomplete_info);
						});
				   return flag;
			   });

		   return input_field;
	   })
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
	return [cb=this->selection_changed]
		(const auto &info)
	{
		editable_comboboxlayoutmanager lm=info.lm;
		x::w::input_field current_selection=info.current_selection;

		standard_combobox_lock lock{lm};

		info.popup_element->hide();
		if (info.selected_flag)
		{
			// If something is already selected, don't touch it.

			input_lock i_lock{current_selection};

			auto [pos1, pos2]=i_lock.pos();

			if (pos1 == pos2)
				current_selection->set(lock.item
						       (info.item_index)
						       .string);
		}
		else // Unselected.
		{
			input_lock i_lock{current_selection};

			if (i_lock.get_unicode() ==
			    lock.item(info.item_index).string)
			{
				current_selection->set("");
			}
		}
		cb({lock, lm, info.item_index, info.selected_flag,
					info.mcguffin});
	};
}

ref<custom_comboboxlayoutmanagerObj::implObj>
new_editable_comboboxlayoutmanager
::create_impl(const create_impl_info &i) const
{
	return ref<editable_comboboxlayoutmanagerObj::implObj>
		::create(i.container_impl, i.style);
}

LIBCXXW_NAMESPACE_END
