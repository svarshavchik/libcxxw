/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "color_picker/color_picker_impl.H"
#include "x/w/impl/container_element.H"
#include "x/w/impl/nonrecursive_visibility.H"

LIBCXXW_NAMESPACE_START

color_pickerObj::handlerObj::handlerObj(const container_impl &parent_container)
	: superclass_t{ parent_container,
		child_element_init_params{"color_picker@libcxx.com"}}
{
}

color_pickerObj::handlerObj::~handlerObj()
{
}

LIBCXXW_NAMESPACE_END
