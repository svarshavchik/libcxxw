/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "panelayoutmanager/pane_peephole_impl.H"

LIBCXXW_NAMESPACE_START

pane_peepholeObj
::pane_peepholeObj(const ref<implObj> &impl,
		   const ref<layoutmanagerObj::implObj> &layout_impl)
	: containerObj{impl, layout_impl},
	  impl{impl}
{
}

pane_peepholeObj::~pane_peepholeObj()=default;

LIBCXXW_NAMESPACE_END
