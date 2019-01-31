/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "peepholed_item_container_impl.H"
#include "peephole/peepholed_element.H"

LIBCXXW_NAMESPACE_START

peepholed_item_containerObj
::peepholed_item_containerObj(const ref<implObj> &impl,
			      const layout_impl &l)
	: superclass_t{impl, l},
	  impl{impl}
{
}

peepholed_item_containerObj::~peepholed_item_containerObj()=default;

dim_t peepholed_item_containerObj::horizontal_increment(ONLY IN_THREAD) const
{
	return impl->font_nominal_width(IN_THREAD);
}

dim_t peepholed_item_containerObj::vertical_increment(ONLY IN_THREAD) const
{
	return impl->font_height(IN_THREAD);
}

size_t peepholed_item_containerObj::peepholed_rows(ONLY IN_THREAD) const
{
	return 0;
}

LIBCXXW_NAMESPACE_END
