/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"

#include "peephole/peephole_gridlayoutmanagerobj.H"
#include "container.H"

LIBCXXW_NAMESPACE_START

container peephole_gridlayoutmanagerObj::get_peephole_container()
{
	return get(0, 0);
}

void peephole_gridlayoutmanagerObj::request_visibility_recursive(IN_THREAD_ONLY,
								 bool flag)
{
	get_peephole_container()->elementObj::impl
		->request_visibility_recursive(IN_THREAD, flag);
}

LIBCXXW_NAMESPACE_END
