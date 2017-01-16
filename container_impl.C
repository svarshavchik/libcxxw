/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "container.H"
#include "x/w/container.H"
#include "layoutmanager.H"

LIBCXXW_NAMESPACE_START

containerObj::implObj::implObj()=default;

containerObj::implObj::~implObj()=default;

void containerObj::implObj
::install_layoutmanager(const ref<layoutmanagerObj::implObj> &impl)
{
	layoutmanager_ptr_t::lock lock(layoutmanager_ptr);

	*lock=impl;
}


void containerObj::implObj::uninstall_layoutmanager()
{
	layoutmanager_ptr_t::lock lock(layoutmanager_ptr);

	*lock=ptr<layoutmanagerObj::implObj>();
}

LIBCXXW_NAMESPACE_END
