/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"

#include "peepholed_toplevel_main_window.H"
#include "peepholed_toplevel_main_window_impl.H"
#include "peephole/peepholed_toplevel_element.H"

LIBCXXW_NAMESPACE_START

peepholed_toplevel_main_windowObj
::peepholed_toplevel_main_windowObj(const ref<implObj> &impl,
				  const new_layoutmanager &layout_factory)
	: toplevel_container_superclass_t(impl, layout_factory),
	  impl(impl)
{
}

peepholed_toplevel_main_windowObj::~peepholed_toplevel_main_windowObj()=default;

void peepholed_toplevel_main_windowObj::recalculate_metrics(IN_THREAD_ONLY)
{
	impl->recalculate_metrics(IN_THREAD);
}

dim_t peepholed_toplevel_main_windowObj::max_width(IN_THREAD_ONLY) const
{
	return impl->data(IN_THREAD).max_width;
}

dim_t peepholed_toplevel_main_windowObj::max_height(IN_THREAD_ONLY) const
{
	return impl->data(IN_THREAD).max_height;
}

dim_t peepholed_toplevel_main_windowObj::horizontal_increment(IN_THREAD_ONLY)
	const
{
	return impl->data(IN_THREAD).horizontal_increment;
}

dim_t peepholed_toplevel_main_windowObj::vertical_increment(IN_THREAD_ONLY) const
{
	return impl->data(IN_THREAD).vertical_increment;
}

LIBCXXW_NAMESPACE_END
