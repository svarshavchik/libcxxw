/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"

#include "peephole/peephole_toplevel_gridlayoutmanagerobj.H"
#include "container.H"
#include "layoutmanager.H"

LIBCXXW_NAMESPACE_START

void peephole_toplevel_gridlayoutmanagerObj::needs_recalculation(IN_THREAD_ONLY)
{
	gridlayoutmanagerObj::implObj::needs_recalculation(IN_THREAD);

	container peephole_container=get(0, 0);

	peephole_container->impl->invoke_layoutmanager
		([&]
		 (const auto &lm)
		 {
			 lm->needs_recalculation(IN_THREAD);
		 });
}


LIBCXXW_NAMESPACE_END
