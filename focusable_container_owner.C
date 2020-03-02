/*
** Copyright 2018-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w//focusable_container_owner.H"

LIBCXXW_NAMESPACE_START

focusable_container_ownerObj
::focusable_container_ownerObj(const ref<containerObj::implObj> &impl,
			       const layout_impl &my_layout_impl,
			       const focusable_impl &f_impl)
	: containerObj{impl, my_layout_impl},
	  focusableObj::ownerObj{f_impl}
	{
	}

focusable_container_ownerObj::~focusable_container_ownerObj()=default;


LIBCXXW_NAMESPACE_END
