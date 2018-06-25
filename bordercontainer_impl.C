/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/impl/bordercontainer_impl.H"
#include "x/w/impl/container.H"
#include "x/w/impl/element.H"
#include "x/w/impl/current_border_impl.H"
#include "screen.H"

LIBCXXW_NAMESPACE_START

bordercontainer_implObj::bordercontainer_implObj()=default;

bordercontainer_implObj::~bordercontainer_implObj()=default;

current_border_impl
bordercontainer_implObj::initial_bordercontainer_border
(const border_arg &initial_border,
 const container_impl &parent_container)
{
	return parent_container->container_element_impl()
		.get_screen()->impl->get_cached_border(initial_border);
}

LIBCXXW_NAMESPACE_END
