/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/booklayoutmanager.H"
#include "booklayoutmanager/bookgridlayoutmanager.H"
#include "booklayoutmanager/booklayoutmanager_impl.H"

LIBCXXW_NAMESPACE_START

bookgridlayoutmanagerObj
::bookgridlayoutmanagerObj(const ref<containerObj::implObj> &container)
	: gridlayoutmanagerObj::implObj(container)
{
}

bookgridlayoutmanagerObj::~bookgridlayoutmanagerObj()=default;

layoutmanager bookgridlayoutmanagerObj::create_public_object()
{
	// The public object's impl object is the intermediate
	// booklayoutmanagerObj::implObj object, so we create it first.

	auto impl=ref<booklayoutmanagerObj::implObj>::create(ref(this));

	return booklayoutmanager::create(impl);
}

LIBCXXW_NAMESPACE_END