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
#include "gridlayoutmanager.H"
#include "menu/menubarlayoutmanager_impl.H"
#include "focus/focusable.H"
#include "x/w/menu.H"

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

void peepholed_toplevel_main_windowObj::implObj
::get_focus_first(IN_THREAD_ONLY, const focusable &f)
{
	// Even though this element wants to be the first one in the tabbing
	// order, we rudely refuse its request, and set it to be the first
	// in the tabbing order after the window's menu bar.

	invoke_layoutmanager
		([&]
		 (const ref<gridlayoutmanagerObj::implObj> &glm)
		 {
			 auto e=glm->get(0, 0);

			 if (!e)
				 return;

			 menubarlayoutmanager mblm=
				 container(e)->get_layoutmanager();

			 menubar_lock lock{mblm};

			 size_t n=mblm->impl->cols(0);
			 if (--n == 0)
			 {
				 // Nothing in the menu bar. Carry on.

				 superclass_t::get_focus_first(IN_THREAD, f);
				 return;
			 }

			 // The last element in the menu bar can be the
			 // divider, if so look at the element before it.

			 if (n == mblm->impl->info(lock).divider_pos &&
			     n-- == 0)
			 {
				 superclass_t::get_focus_first(IN_THREAD, f);
			 }

			 menu m=mblm->impl->get(0, n);

			 // We need to make sure that, first, this element's
			 // tabbing order is correct, then the new element's
			 // tabbing order will be after it.
			 mblm->impl->fix_order(IN_THREAD, m);

			 get_focus_after_in_thread(IN_THREAD, f, m);
		 });
}

LIBCXXW_NAMESPACE_END
