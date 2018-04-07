/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "combobox/custom_comboboxlayoutmanager.H"
#include "combobox/custom_combobox_container_impl.H"
#include "combobox/custom_combobox_popup_containerobj.H"
#include "x/w/impl/container.H"

LIBCXXW_NAMESPACE_START

custom_comboboxlayoutmanagerObj::implObj
::implObj(const ref<custom_combobox_containerObj::implObj> &container_impl,
	  const new_custom_comboboxlayoutmanager &style)
	: gridlayoutmanagerObj::implObj(container_impl),
	container_impl(container_impl),
	selection_changed(style.get_selection_changed())
{
}

custom_comboboxlayoutmanagerObj::implObj::~implObj()=default;

layoutmanager custom_comboboxlayoutmanagerObj::implObj::create_public_object()
{
	return custom_comboboxlayoutmanager
		::create(ref<implObj>(this),
			 container_impl->popup_container->layout_impl);

}

LIBCXXW_NAMESPACE_END
