/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/listlayoutmanager.H"
#include "listlayoutmanager/listcontainer_impl.H"
#include "themedim.H"
#include "element_screen.H"
#include "x/w/listlayoutmanager.H"

LIBCXXW_NAMESPACE_START

listlayoutmanagerObj::implObj
::implObj(const ref<listcontainerObj::implObj> &container_impl,
	  const new_listlayoutmanager &style)
	: gridlayoutmanagerObj::implObj(container_impl),
	container_impl(container_impl),
	style(style.layout_style),
	columns(style.columns)
{
}

listlayoutmanagerObj::implObj::~implObj()=default;

layoutmanager listlayoutmanagerObj::implObj::create_public_object()
{
	return listlayoutmanager::create(ref<implObj>(this));
}

LIBCXXW_NAMESPACE_END
