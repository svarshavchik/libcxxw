/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"

#include "peephole/peephole_toplevel_gridlayoutmanagerobj.H"
#include "peephole/peephole_impl.H"
#include "x/w/scrollbar.H"
#include "x/w/impl/container.H"
#include "x/w/impl/layoutmanager.H"

LIBCXXW_NAMESPACE_START

void peephole_toplevel_gridlayoutmanagerObj::needs_recalculation(ONLY IN_THREAD)
{
	peephole_gridlayoutmanagerObj::needs_recalculation(IN_THREAD);

	my_peephole->impl->invoke_layoutmanager
		([&]
		 (const auto &lm)
		 {
			 lm->needs_recalculation(IN_THREAD);
		 });
}


LIBCXXW_NAMESPACE_END
