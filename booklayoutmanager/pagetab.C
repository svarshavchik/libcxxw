/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "booklayoutmanager/pagetab_impl.H"

LIBCXXW_NAMESPACE_START

pagetabObj::pagetabObj(const ref<implObj> &impl,
		       const layout_impl &container_layout_impl)
	: containerObj{impl, container_layout_impl},
	  hotspotObj(impl),
	  focusableObj::ownerObj{impl},
	impl{impl}
	{
	}

pagetabObj::~pagetabObj()=default;

LIBCXXW_NAMESPACE_END
