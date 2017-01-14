/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "container.H"
#include "x/w/container.H"
#include "x/w/new_layoutmanager.H"
#include "layoutmanager.H"

LIBCXXW_NAMESPACE_START

containerObj::containerObj(const ref<implObj> &impl,
			   const new_layoutmanager &layout_factory)
	: elementObj(impl),
	  impl(impl),
	  manager(layoutmanager::create(layout_factory, impl))
{
}

containerObj::~containerObj()
{
}

layoutmanager containerObj::get_layoutmanager()
{
	return manager;
}

const_layoutmanager containerObj::get_layoutmanager() const
{
	return manager;
}

LIBCXXW_NAMESPACE_END
