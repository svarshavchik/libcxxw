/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "booklayoutmanager/pagetab_impl.H"

LIBCXXW_NAMESPACE_START

pagetabObj::pagetabObj(const ref<implObj> &impl,
		       const ref<layoutmanagerObj::implObj> &layout_impl)
	: containerObj{impl, layout_impl},
	  hotspotObj(impl),
	  focusableObj::ownerObj{impl},
	impl{impl}
	{
	}

pagetabObj::~pagetabObj()=default;

LIBCXXW_NAMESPACE_END
