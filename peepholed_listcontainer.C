/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "peephole/peepholed_element.H"
#include "peepholed_listcontainer.H"
#include "x/w/impl/theme_font_element.H"

LIBCXXW_NAMESPACE_START

peepholed_listcontainerObj
::peepholed_listcontainerObj(const ref<listcontainer_pseudo_implObj> &impl,
			     const focusable_impl &focusable_impl,
			     const layout_impl &list_impl)
	: superclass_t{focusable_impl, impl, list_impl},
	  impl{impl}
{
}

peepholed_listcontainerObj::~peepholed_listcontainerObj()=default;

dim_t peepholed_listcontainerObj::horizontal_increment(ONLY IN_THREAD) const
{
	return impl->rowsize(IN_THREAD).with_padding;
	// Same horizontal and vertical
}

dim_t peepholed_listcontainerObj::vertical_increment(ONLY IN_THREAD) const
{
	return impl->rowsize(IN_THREAD).with_padding;
}

size_t peepholed_listcontainerObj::peepholed_rows(ONLY IN_THREAD) const
{
	return impl->rows(IN_THREAD);
}

LIBCXXW_NAMESPACE_END
