/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/list_container_impl.H"
#include "container_element.H"

LIBCXXW_NAMESPACE_START

list_container_implObj
::list_container_implObj(const ref<containerObj::implObj> &parent)
	: superclass_t(parent)
{
}

list_container_implObj::~list_container_implObj()=default;

///////////////////////////////////////////////////////////////////////////


LIBCXXW_NAMESPACE_END
