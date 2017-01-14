/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "layoutmanager.H"
#include "x/w/new_layoutmanager.H"
#include "container.H"

LIBCXXW_NAMESPACE_START

layoutmanagerObj::layoutmanagerObj(const new_layoutmanager &layout_factory,
				   const ref<containerObj::implObj>
				   &container_impl)
	: impl(layout_factory->create(container_impl))
{
	impl->container_impl->install_layoutmanager(impl);
}

layoutmanagerObj::~layoutmanagerObj()
{
	impl->container_impl->uninstall_layoutmanager();
}

LIBCXXW_NAMESPACE_END
