/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "peephole/peepholed_element.H"
#include "peepholed_listcontainer_impl.H"
#include "peepholed_listcontainer.H"
#include "reference_font_element.H"

LIBCXXW_NAMESPACE_START

peepholed_listcontainerObj
::peepholed_listcontainerObj(const ref<implObj> &impl,
			     const focusable_impl &focusable_impl,
			     const container_impl &container_impl,
			     const ref<layoutmanagerObj::implObj> &list_impl)
	: superclass_t(focusable_impl, container_impl, list_impl),
	  impl(impl)
{
}

peepholed_listcontainerObj::~peepholed_listcontainerObj()=default;

dim_t peepholed_listcontainerObj::horizontal_increment(ONLY IN_THREAD) const
{
	return impl->list_reference_font().font_nominal_width(IN_THREAD);
}

dim_t peepholed_listcontainerObj::vertical_increment(ONLY IN_THREAD) const
{
	return impl->rowsize(IN_THREAD);
}

LIBCXXW_NAMESPACE_END
