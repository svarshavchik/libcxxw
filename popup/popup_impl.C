/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "popup/popup_impl.H"
#include "popup/popup_handler.H"
#include "generic_window_handler.H"
#include "element_screen.H"
#include <xcb/xproto.h>

LIBCXXW_NAMESPACE_START

// Set window manager hints to indicate whose top level display element
// we're a popup for.

static void set_parent_window(const ref<generic_windowObj::handlerObj>
			      &popup_handler, xcb_window_t parent_window_id)
{
	popup_handler->update_wm_hints
		([&]
		 (xcb_icccm_wm_hints_t &hints)
		 {
			 xcb_icccm_wm_hints_set_window_group
				 (&hints, parent_window_id);

			 xcb_icccm_wm_hints_set_input(&hints, 0);
		 });

	auto c=popup_handler->screenref->get_connection()->impl->info->conn;

	xcb_icccm_set_wm_transient_for(c,
				       popup_handler->id(),
				       parent_window_id);

}


popupObj::implObj::implObj(IN_THREAD_ONLY,
			   const ref<handlerObj> &handler,
			   const ref<generic_windowObj::handlerObj> &parent)
	: generic_windowObj::implObj(handler),
	  handler(handler)
{
	set_parent_window(handler, parent->id());

	values_and_mask vm(XCB_CW_OVERRIDE_REDIRECT, 1);

	auto c=handler->screenref->get_connection()->impl->info->conn;

	xcb_change_window_attributes(c,
				     handler->id(),
				     vm.mask(),
				     vm.values().data());

}

popupObj::implObj::~implObj()
{
	set_parent_window(handler, XCB_NONE);
}

LIBCXXW_NAMESPACE_END
