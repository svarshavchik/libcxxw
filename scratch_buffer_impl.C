/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/picture.H"
#include "x/w/pixmap.H"
#include "x/w/impl/scratch_buffer.H"
#include <x/exception.H>

LIBCXXW_NAMESPACE_START

scratch_bufferObj::implObj::implObj(const pixmap &pm)
	: cached_picture(pm, pm->create_picture(), pm->create_gc())
{
}

scratch_bufferObj::implObj::~implObj()=default;

void scratch_bufferObj::implObj
::do_get(dim_t minimum_width,
	 dim_t minimum_height,
	 const function<void (const picture &, const pixmap &,
			      const gc &)> &callback)
{
	if (minimum_width == dim_t::infinite() ||
	    minimum_height == dim_t::infinite())
		throw EXCEPTION("Internal error: infinite pixmap dimension.");

	cached_picture_t::lock lock(cached_picture);

	if (minimum_width > lock->pm->get_width() ||
	    minimum_height > lock->pm->get_height())
	{
		dim_t w=dim_t::truncate(minimum_width * 2);
		dim_t h=dim_t::truncate(minimum_height * 2);

		if (w == dim_t::infinite())
			w -= 1;
		if (h == dim_t::infinite())
			h -= 1;
		auto new_pm=lock->pm->create_pixmap(w, h);
		auto new_pic=new_pm->create_picture();
		auto new_gc=new_pm->create_gc();

		lock->pm=new_pm;
		lock->pic=new_pic;
		lock->graphic_context=new_gc;
	}
	callback(lock->pic, lock->pm, lock->graphic_context);
}

LIBCXXW_NAMESPACE_END
