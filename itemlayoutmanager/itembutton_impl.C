/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "itembutton_impl.H"
#include "x/w/impl/container_element.H"
#include "x/w/impl/always_visible_element.H"

LIBCXXW_NAMESPACE_START

itembuttonObj::implObj::implObj(const container_impl &parent_container,
				const child_element_init_params &init_params)
	: superclass_t{parent_container, init_params}
{
}

itembuttonObj::implObj::~implObj()=default;

LIBCXXW_NAMESPACE_END
