/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "peephole_impl.H"
#include "peephole_layoutmanager_impl.H"
#include "peephole.H"

LIBCXXW_NAMESPACE_START

peepholeObj::peepholeObj(const ref<implObj> &impl,
			 const ref<layoutmanager_implObj> &layout)
	: containerObj(impl, layout), impl(impl)
{
}

peepholeObj::~peepholeObj()=default;

LIBCXXW_NAMESPACE_END
