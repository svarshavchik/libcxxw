/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "combobox/editable_comboboxlayoutmanager.H"
#include "combobox/custom_combobox_container_impl.H"
#include "combobox/custom_combobox_popup_container.H"

LIBCXXW_NAMESPACE_START

editable_comboboxlayoutmanagerObj::implObj
::implObj(const ref<custom_combobox_containerObj::implObj> &container_impl,
	  const new_editable_comboboxlayoutmanager &style)
	: standard_comboboxlayoutmanagerObj::implObj(container_impl, style,
						     // Dummy selection changed
						     // callback for the parent
						     // standard combobox class.
						     [](const auto &){}),
	selection_changed{style.selection_changed}
{
}

editable_comboboxlayoutmanagerObj::implObj::~implObj()=default;

layoutmanager editable_comboboxlayoutmanagerObj::implObj::create_public_object()
{
	return editable_comboboxlayoutmanager
		::create(ref<implObj>(this),
			 container_impl->popup_container->layout_impl);
}

LIBCXXW_NAMESPACE_END
