/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "combobox/standard_comboboxlayoutmanager.H"
#include "combobox/custom_combobox_container_impl.H"
#include "combobox/custom_combobox_popup_container.H"

LIBCXXW_NAMESPACE_START

standard_comboboxlayoutmanagerObj::implObj
::implObj(const ref<custom_combobox_containerObj::implObj> &container_impl,
	  const new_custom_comboboxlayoutmanager &style,
	  const standard_combobox_selection_changed_t &selection_changed,
	  bool selection_changed_enabled)
	: custom_comboboxlayoutmanagerObj::implObj{container_impl, style,
						   false},
	  selection_changed{selection_changed},
	  selection_changed_enabled{selection_changed_enabled}
{
}

standard_comboboxlayoutmanagerObj::implObj::~implObj()=default;

layoutmanager standard_comboboxlayoutmanagerObj::implObj::create_public_object()
{
	return standard_comboboxlayoutmanager
		::create(ref<implObj>(this),
			 combo_container_impl->popup_container
			 ->listlayout_impl);

}

void standard_comboboxlayoutmanagerObj::implObj
::update_items_if_needed(std::vector<list_item_param> &items)
{
}

LIBCXXW_NAMESPACE_END
