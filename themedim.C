/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "themedim.H"
#include "screen.H"
#include "defaulttheme.H"

LIBCXXW_NAMESPACE_START

static dim_t compute_dim(const auto &screen_impl,
			 const dim_arg &dimname,
			 auto width_or_height)
{
	current_theme_t::lock lock{screen_impl->current_theme};

	return ((**lock).*width_or_height)(dimname);
}

themedimObj::themedimObj(const dim_arg &dimname,
			 const ref<screenObj::implObj> &screen_impl,
			 theme_width_or_height width_or_height)
	: dimname(dimname),
	  pixels_thread_only(compute_dim(screen_impl, dimname,
					 width_or_height)),
	  width_or_height(width_or_height)
{
}

themedimObj::~themedimObj()=default;

void themedimObj::initialize(IN_THREAD_ONLY,
			     const ref<screenObj::implObj> &screen_impl)
{
	// Recalculate now we're in the connection thread.

	pixels(IN_THREAD)=compute_dim(screen_impl, dimname, width_or_height);
}

void themedimObj::theme_updated(IN_THREAD_ONLY,
				const defaulttheme &new_theme)
{
	pixels(IN_THREAD)=((*new_theme).*width_or_height)(dimname);
}

LIBCXXW_NAMESPACE_END
