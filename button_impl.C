/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "button.H"
#include "x/w/impl/container_element.H"
#include "x/w/impl/container_visible_element.H"
#include "x/w/impl/bordercontainer_element.H"
#include "x/w/impl/richtext/richtext.H"

LIBCXXW_NAMESPACE_START

buttonObj::implObj::implObj(const border_arg &left_border,
			    const border_arg &right_border,
			    const border_arg &top_border,
			    const border_arg &bottom_border,
			    const container_impl &container,
			    const child_element_init_params &init_params)
	: superclass_t{container->get_window_handler(),
		       left_border, right_border,
		       top_border, bottom_border,
		       richtextptr{},
		       0, 0, 0,
		       container, init_params}
{
}

buttonObj::implObj::~implObj()=default;

LIBCXXW_NAMESPACE_END
