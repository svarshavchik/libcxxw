/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "button.H"
#include "x/w/button.H"
#include "x/w/button_appearance.H"
#include "x/w/impl/container_element.H"
#include "x/w/impl/container_visible_element.H"
#include "x/w/impl/bordercontainer_element.H"
#include "x/w/impl/richtext/richtext.H"

LIBCXXW_NAMESPACE_START

buttonObj::implObj::implObj(const button_config &config,
			    const container_impl &container,
			    const child_element_init_params &init_params)
	: superclass_t{container->get_window_handler(),
		config.appearance->left_border, config.appearance->right_border,
		config.appearance->top_border, config.appearance->bottom_border,
		richtextptr{},
		0, 0, 0, container, init_params}
{
}

buttonObj::implObj::~implObj()=default;

LIBCXXW_NAMESPACE_END
