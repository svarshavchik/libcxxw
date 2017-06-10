/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/listlayoutmanager.H"
#include "listlayoutmanager/listcontainer_impl.H"
#include "themedim.H"
#include "element_screen.H"

LIBCXXW_NAMESPACE_START

listlayoutmanagerObj::implObj
::implObj(const ref<listcontainerObj::implObj> &container_impl,
	  const listlayoutstyle &style,
	  size_t columns)
	: gridlayoutmanagerObj::implObj(container_impl),
	container_impl(container_impl),
	style(style),
	columns(columns)
{
}

listlayoutmanagerObj::implObj::~implObj()=default;

layoutmanager listlayoutmanagerObj::implObj::create_public_object()
{
	return listlayoutmanager::create(ref<implObj>(this));
}

LIBCXXW_NAMESPACE_END
