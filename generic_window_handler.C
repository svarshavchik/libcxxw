/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "generic_window_handler.H"
#include "x/w/pictformat.H"

LIBCXXW_NAMESPACE_START

generic_windowObj::handlerObj
::handlerObj(IN_THREAD_ONLY,
	     const constructor_params &params)

	// This sets up the xcb_window_t

	: window_handlerObj(IN_THREAD),
	  drawableObj::implObj(window_handlerObj::id(),
			       params.drawable_pictformat),
	  background_colorObj(params.initial_background_color)
{
}

generic_windowObj::handlerObj::~handlerObj() noexcept=default;

void generic_windowObj::handlerObj::create_picture(IN_THREAD_ONLY)
{
}


LIBCXXW_NAMESPACE_END
