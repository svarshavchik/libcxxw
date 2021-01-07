/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "color_picker/color_picker_selector_impl.H"

LIBCXXW_NAMESPACE_START

color_picker_selectorObj::implObj
::implObj(const container_impl &parent,
	  const child_element_init_params &init_params)
	: superclass_t{parent, init_params}
{
}

color_picker_selectorObj::implObj::~implObj()=default;

LIBCXXW_NAMESPACE_END
