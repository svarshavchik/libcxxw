/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "generic_window_handler.H"

LIBCXXW_NAMESPACE_START

busy::busy(const ref<generic_windowObj::handlerObj> &w) : w(w)
{
	w->busy_count.refadd(1);
}

busy::busy(const busy &c) : w(c.w)
{
	w->busy_count.refadd(1);
}

busy::~busy()
{
	w->busy_count.refadd(-1);
}

LIBCXXW_NAMESPACE_END
