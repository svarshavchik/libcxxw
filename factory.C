/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/factory.H"
#include "container.H"
#include "child_element.H"

LIBCXXW_NAMESPACE_START

factoryObj::factoryObj(const ref<containerObj::implObj> &container_impl)
	: container_impl(container_impl)
{
}

factoryObj::~factoryObj()=default;

element factoryObj::create_empty_element(const metrics::axis &horiz_axis,
					 const metrics::axis &vert_axis)
{
	auto ce=child_element::create(container_impl, horiz_axis, vert_axis);

	auto e=element::create(ce);

	created(ce);

	return e;
}


LIBCXXW_NAMESPACE_END
