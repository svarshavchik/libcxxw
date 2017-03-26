/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef new_layoutmanager_H
#define new_layoutmanager_H

#include "x/w/new_layoutmanager.H"
#include "x/w/layoutmanagerobj.H"
#include "x/w/containerobj.H"
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

struct LIBCXX_HIDDEN new_layoutmanager_results {

	ref<layoutmanagerObj::implObj> layout_manager_impl;
};

struct LIBCXX_HIDDEN new_layoutmanager_info {

	ref<containerObj::implObj> container_impl;
};

LIBCXXW_NAMESPACE_END

#endif
