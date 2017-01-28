/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "generic_window.H"
#include "generic_window_handler.H"
#include "drawable.H"
#include "x/w/screen.H"

LOG_CLASS_INIT(LIBCXX_NAMESPACE::w::generic_windowObj);

LIBCXXW_NAMESPACE_START

generic_windowObj::generic_windowObj(const ref<implObj> &impl,
				     const new_layoutmanager &layout_factory)
	: containerObj(impl->handler, layout_factory),
	  drawableObj(impl->handler),
	  impl(impl)
{
}

generic_windowObj::~generic_windowObj()=default;

LIBCXXW_NAMESPACE_END
