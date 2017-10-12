/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "textlist_container_impl.H"
#include "container_element.H"

LIBCXXW_NAMESPACE_START

textlist_container_implObj
::textlist_container_implObj(const ref<containerObj::implObj> &parent)
	: superclass_t(parent)
{
}

textlist_container_implObj::~textlist_container_implObj()=default;

///////////////////////////////////////////////////////////////////////////


LIBCXXW_NAMESPACE_END
