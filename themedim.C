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
			 const std::string &dimname)
{
	current_theme_t::lock lock{screen_impl->current_theme};

	return (*lock)->get_theme_dim_t(dimname);
}

themedimObj::themedimObj(const std::string &dimname,
			 const ref<screenObj::implObj> &screen_impl)
	: dimname(dimname),
	pixels_thread_only(compute_dim(screen_impl, dimname))
{
}

themedimObj::~themedimObj()=default;

void themedimObj::initialize(IN_THREAD_ONLY,
			     const ref<screenObj::implObj> &screen_impl)
{
	// Recalculate now we're in the connection thread.

	pixels(IN_THREAD)=compute_dim(screen_impl, dimname);
}

void themedimObj::theme_updated(IN_THREAD_ONLY,
				const defaulttheme &new_theme)
{
	pixels(IN_THREAD)=new_theme->get_theme_dim_t(dimname);
}

LIBCXXW_NAMESPACE_END
