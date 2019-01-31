/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "image_button_internal_impl.H"
#include "image.H"
#include "x/w/button_event.H"
#include "x/w/key_event.H"

LIBCXXW_NAMESPACE_START

image_button_internalObj::image_button_internalObj(const ref<implObj> &impl)
	: imageObj(impl),
	  impl(impl)
{
}

image_button_internalObj::~image_button_internalObj()=default;

focusable_impl image_button_internalObj::get_impl() const
{
	return impl;
}

void image_button_internalObj::resize(ONLY IN_THREAD,
				      dim_t w, dim_t h,
				      icon_scale scale)
{
	impl->resize(IN_THREAD, w, h, scale);
	impl->set_image_number(IN_THREAD, {}, impl->get_image_number());
}

LIBCXXW_NAMESPACE_END
