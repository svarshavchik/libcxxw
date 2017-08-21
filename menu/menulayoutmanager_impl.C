/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "menu/menulayoutmanager_impl.H"
#include "menu/menuitemextrainfo.H"
#include "listlayoutmanager/listitemcontainer.H"
#include "listlayoutmanager/listlayoutmanager.H"
#include "x/w/menulayoutmanager.H"

LIBCXXW_NAMESPACE_START

menulayoutmanagerObj::implObj
::implObj(const ref<peepholed_toplevel_listcontainer_implObj> &container_impl,
	  const ref<listcontainerObj::implObj> &listcontainer_impl,
	  const new_listlayoutmanager &style)
	: peepholed_toplevel_listcontainer_layoutmanager_implObj
	  (container_impl, listcontainer_impl, style)
{
}

menulayoutmanagerObj::implObj::~implObj()=default;

layoutmanager menulayoutmanagerObj::implObj::create_public_object()
{
	return menulayoutmanager::create(ref<implObj>(this));
}


menuitemextrainfo menulayoutmanagerObj::implObj
::get_extrainfo(listlayoutmanagerObj &lm,
		size_t item_number)
{
	// Last column is the extra_info we're looking for.

	listitemcontainer lic=lm.item(item_number, lm.impl->columns-1);

	return lic->get();
}

LIBCXXW_NAMESPACE_END
