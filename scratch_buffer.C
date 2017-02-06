/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "scratch_buffer.H"
#include "x/w/picture.H"
#include "x/w/pixmap.H"

LIBCXXW_NAMESPACE_START

scratch_bufferObj::scratch_bufferObj(const ref<implObj> &impl)
	: impl(impl)
{
}

scratch_bufferObj::~scratch_bufferObj()=default;

void scratch_bufferObj::do_get(dim_t minimum_width,
			       dim_t minimum_height,
			       const function<void (const picture &,
						    const pixmap &,
						    const gc &)> &callback)
{
	impl->do_get(minimum_width, minimum_height, callback);
}

const_picture scratch_bufferObj::get_picture() const
{
	return impl->get_picture();
}

const_pixmap scratch_bufferObj::get_pixmap() const
{
	return impl->get_pixmap();
}

LIBCXXW_NAMESPACE_END
