/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/factory.H"
#include "x/w/impl/container.H"
#include "messages.H"
#include "x/w/impl/child_element.H"
#include <x/exception.H>

LIBCXXW_NAMESPACE_START

factoryObj::factoryObj()=default;

factoryObj::~factoryObj()=default;

container_impl factoryObj::last_container_impl()
{
	return get_container_impl();
}

void factoryObj::created_internally(const element &e)
{
	ref<child_elementObj> impl=e->impl;

	if (impl->child_container != last_container_impl())
		throw EXCEPTION(_("Internal error: child element added to the wrong container"));

	created(e);
}

LIBCXXW_NAMESPACE_END
