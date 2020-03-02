/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "combobox/editable_comboboxlayoutmanager.H"
#include "combobox/custom_combobox_container_impl.H"
#include "combobox/custom_combobox_popup_container.H"

LIBCXXW_NAMESPACE_START

static standard_combobox_selection_changed_t noop_standard_selection_changed=
	[]
	(ONLY IN_THREAD, const auto &)
{
};

editable_comboboxlayoutmanagerObj::implObj
::implObj(const ref<custom_combobox_containerObj::implObj> &container_impl,
	  const new_editable_comboboxlayoutmanager &style)
	: standard_comboboxlayoutmanagerObj::implObj
	{container_impl, style,
	 // Dummy selection changed
	 // callback for the parent
	 // standard combobox class.
	 noop_standard_selection_changed,
	 false},
	  selection_changed{style.selection_changed}
{
}

editable_comboboxlayoutmanagerObj::implObj::~implObj()=default;

layoutmanager editable_comboboxlayoutmanagerObj::implObj::create_public_object()
{
	return editable_comboboxlayoutmanager
		::create(ref<implObj>(this),
			 combo_container_impl->popup_container
			 ->listlayout_impl);
}

void editable_comboboxlayoutmanagerObj::implObj
::update_items_if_needed(std::vector<list_item_param> &items)
{
	for (auto &item:items)
	{
		if (!std::holds_alternative<text_param>(item))
			continue;

		text_param &t=std::get<text_param>(item);

		t(U" ");
	}
}

LIBCXXW_NAMESPACE_END
