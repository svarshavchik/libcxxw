/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/picture.H"
#include "x/w/pixmap.H"
#include "x/w/screen.H"
#include "x/w/pictformat.H"
#include "x/w/impl/scratch_buffer.H"
#include <x/exception.H>

LIBCXXW_NAMESPACE_START

scratch_bufferObj::implObj::implObj(const const_pictformat &scratch_pictformat,
				    const screen &scratch_screen)

	: scratch_pictformat{scratch_pictformat},
	  scratch_screen{scratch_screen},
	  cached_picture{std::nullopt}
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

	if (*lock)
	{
		auto &info=**lock;

		auto current_width=info.pm->get_width();
		auto current_height=info.pm->get_height();

		// If the current scratch buffer is big enough, great!
		if (minimum_width <= current_width &&
		    minimum_height <= current_height)
		{
			callback(info.pic, info.pm, info.graphic_context);
			return;
		}

		// Hmm, let's leave room for growth.

		if (minimum_width > current_width)
			current_width = dim_t::truncate(minimum_width
							+ (minimum_width-
							   current_width)/4);
		else
			minimum_width=current_width;

		if (minimum_height > current_height)
			current_height = dim_t::truncate(minimum_height
							+ (minimum_height-
							   current_height)/4);
		else
			minimum_height=current_height;
	}

	lock->reset(); // Politely release the current resources, first.

	auto new_pm=scratch_screen->create_pixmap(scratch_pictformat,
						  minimum_width,
						  minimum_height);

	auto new_pic=new_pm->create_picture();
	auto new_gc=new_pm->create_gc();

	(*lock)={new_pm, new_pic, new_gc};

	callback(new_pic, new_pm, new_gc);
}

LIBCXXW_NAMESPACE_END
