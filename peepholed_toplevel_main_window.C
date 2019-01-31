/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"

#include "peepholed_toplevel_main_window.H"
#include "peepholed_toplevel_main_window_impl.H"
#include "peephole/peepholed_toplevel_element.H"
#include "x/w/impl/layoutmanager.H"

LIBCXXW_NAMESPACE_START

peepholed_toplevel_main_windowObj
::peepholed_toplevel_main_windowObj(const ref<implObj> &impl,
				    const layout_impl &container_layout_impl)
	: toplevel_container_superclass_t{impl, container_layout_impl},
	  impl{impl}
{
}

peepholed_toplevel_main_windowObj::~peepholed_toplevel_main_windowObj()=default;

void peepholed_toplevel_main_windowObj
::recalculate_peepholed_metrics(ONLY IN_THREAD, const screen &s)
{
	impl->recalculate_metrics(IN_THREAD);
}

dim_t peepholed_toplevel_main_windowObj::max_width(ONLY IN_THREAD) const
{
	return impl->data(IN_THREAD).max_width;
}

dim_t peepholed_toplevel_main_windowObj::max_height(ONLY IN_THREAD) const
{
	return impl->data(IN_THREAD).max_height;
}

dim_t peepholed_toplevel_main_windowObj::horizontal_increment(ONLY IN_THREAD)
	const
{
	return impl->reference_font::font_nominal_width(IN_THREAD);
}

dim_t peepholed_toplevel_main_windowObj::vertical_increment(ONLY IN_THREAD) const
{
	return impl->reference_font::font_height(IN_THREAD);
}

size_t peepholed_toplevel_main_windowObj::peepholed_rows(ONLY IN_THREAD) const
{
	return 0;
}

LIBCXXW_NAMESPACE_END
