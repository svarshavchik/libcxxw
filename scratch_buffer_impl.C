/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/picture.H"
#include "x/w/pixmap.H"
#include "x/w/screen.H"
#include "x/w/pictformat.H"
#include "scratch_buffer.H"
#include <x/exception.H>

LIBCXXW_NAMESPACE_START

scratch_bufferObj::implObj::implObj(const const_pictformat &scratch_pictformat,
				    const std::string &identifier,
				    const screen &scratch_screen)

	: scratch_pictformat{scratch_pictformat},
#if SCRATCH_BUFFER_DEBUG
	  identifier{identifier},
#endif
	  scratch_screen{scratch_screen},
	  cached_picture{std::nullopt}
{
#if SCRATCH_BUFFER_DEBUG
	std::cout << "scratch buffer " << identifier << "@" << this
		  << " created" << std::endl;
#endif
}

scratch_bufferObj::implObj::~implObj()
{
#if SCRATCH_BUFFER_DEBUG
	std::cout << "scratch buffer " << identifier << "@" << this
		  << " destroyed" << std::endl;
#endif
}

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

#if SCRATCH_BUFFER_DEBUG
		std::cout << "scratch buffer " << identifier << "@" << this
			  << ": too small for "
			  << minimum_width << "x"
			  << minimum_height << std::endl;
#endif

		// Hmm, let's leave room for growth. Add 1/4th of the
		// increase as an extra buffer. However if this pushes
		// us past the size of the screen, limit at that. It's unlikely
		// that we'll grow past the screen's size.

		if (minimum_width > current_width)
		{
			current_width = dim_t::truncate(minimum_width
							+ (minimum_width-
							   current_width)/4);

			if (current_width > scratch_screen->width_in_pixels())
				current_width=minimum_width;
		}

		minimum_width=current_width;

		if (minimum_height > current_height)
		{
			current_height = dim_t::truncate(minimum_height
							+ (minimum_height-
							   current_height)/4);

			if (current_height > scratch_screen->height_in_pixels())
				current_height=minimum_height;
		}

		minimum_height=current_height;
	}

	lock->reset(); // Politely release the current resources, first.

#if SCRATCH_BUFFER_DEBUG
	std::cout << "scratch buffer " << identifier << "@" << this
		  << ": resized to "
		  << minimum_width << "x"
		  << minimum_height << std::endl;
#endif

	auto new_pm=scratch_screen->create_pixmap(scratch_pictformat,
						  minimum_width,
						  minimum_height);

	auto new_pic=new_pm->create_picture();
	auto new_gc=new_pm->create_gc();

	(*lock)={new_pm, new_pic, new_gc};

	callback(new_pic, new_pm, new_gc);
}

LIBCXXW_NAMESPACE_END
