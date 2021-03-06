/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/impl/themedim.H"
#include "x/w/impl/themedim_element.H"
#include "screen.H"
#include "generic_window_handler.H"
#include "defaulttheme.H"

LIBCXXW_NAMESPACE_START

static dim_t compute_dim(const ref<screenObj::implObj> &screen_impl,
			 const dim_arg &dimname,
			 themedimaxis width_or_height)
{
	current_theme_t::lock lock{screen_impl->current_theme};

	return (*lock)->get_theme_dim_t(dimname, width_or_height);
}

themedimObj::themedimObj(const dim_arg &dimname,
			 themedimaxis width_or_height,
			 const ref<screenObj::implObj> &screen_impl)
	: dimname{dimname},
	  width_or_height{width_or_height},
	  pixels_thread_only{compute_dim(screen_impl, this->dimname,
					 this->width_or_height)}
{
}

themedimObj::~themedimObj()=default;

void themedimObj::initialize(ONLY IN_THREAD,
			     const ref<screenObj::implObj> &screen_impl)
{
	// Recalculate now we're in the connection thread.

	pixels(IN_THREAD)=compute_dim(screen_impl, dimname, width_or_height);
}

void themedimObj::update(ONLY IN_THREAD,
			 const dim_arg &new_dimname,
			 const const_defaulttheme &current_theme)
{
	dimname=new_dimname;
	theme_updated(IN_THREAD, current_theme);
}

void themedimObj::theme_updated(ONLY IN_THREAD,
				const const_defaulttheme &new_theme)
{
	pixels(IN_THREAD)=new_theme->get_theme_dim_t(dimname, width_or_height);
}

LIBCXXW_NAMESPACE_END
