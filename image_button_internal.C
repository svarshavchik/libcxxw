/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "image_button_internal_impl.H"
#include "image.H"

LIBCXXW_NAMESPACE_START

image_button_internalObj::image_button_internalObj(const ref<implObj> &impl)
	: imageObj(impl),
	  impl(impl)
{
}

image_button_internalObj::~image_button_internalObj()=default;

ref<focusableImplObj> image_button_internalObj::get_impl() const
{
	return impl;
}


LIBCXXW_NAMESPACE_END
