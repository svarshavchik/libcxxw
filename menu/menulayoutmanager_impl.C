/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "menu/menulayoutmanager_impl.H"
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

LIBCXXW_NAMESPACE_END
