/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"

#include "peepholed_toplevel_main_window_impl.H"
#include "peephole/peepholed_toplevel_element.H"
#include "generic_window_handler.H"
#include "reference_font_element.H"
#include "layoutmanager.H"
#include "always_visible.H"

LIBCXXW_NAMESPACE_START

peepholed_toplevel_main_windowObj::implObj
::implObj(const ref<containerObj::implObj> &parent_container)
	: superclass_t(parent_container->get_element_impl().label_theme_font(),
		       parent_container)
{
}

peepholed_toplevel_main_windowObj::implObj::~implObj()=default;

void peepholed_toplevel_main_windowObj::implObj::initialize(IN_THREAD_ONLY)
{
	superclass_t::initialize(IN_THREAD);
	recalculate_metrics(IN_THREAD);

	child_container->invoke_layoutmanager
		([&]
		 (const auto &lm)
		 {
			 // Needs to take notice.

			 lm->needs_recalculation(IN_THREAD);
		 });
}

void peepholed_toplevel_main_windowObj::implObj
::recalculate_metrics(IN_THREAD_ONLY)
{
	if (!elementObj::implObj::data(IN_THREAD).initialized)
		return;

	// Don't tell the window manager that our minimum
	// dimensions exceed usable workarea size.

	// Subtract frame size from total workarea size.
	// That's our cap.

	auto &fe=get_element_impl().get_window_handler()
		.frame_extents(IN_THREAD);

	auto usable_workarea_width=
		fe.workarea.width-fe.left-fe.right;

	auto usable_workarea_height=
		fe.workarea.height-fe.top-fe.bottom;

	auto &d=data(IN_THREAD);
	d.max_width=dim_t::truncate(usable_workarea_width);
	d.max_height=dim_t::truncate(usable_workarea_height);
}

LIBCXXW_NAMESPACE_END
