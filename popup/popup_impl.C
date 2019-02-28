/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "popup/popup.H"
#include "popup/popup_impl.H"
#include "popup/popup_handler.H"
#include "generic_window_handler.H"
#include "screen.H"
#include <xcb/xproto.h>

LIBCXXW_NAMESPACE_START

// Set window manager hints to indicate whose top level display element
// we're a popup for.

void set_parent_window_of(const ref<generic_windowObj::handlerObj> &handler,
			  xcb_window_t parent_window_id)
{
#if 0
	handler->update_wm_hints
		([&]
		 (xcb_icccm_wm_hints_t &hints)
		 {
			 xcb_icccm_wm_hints_set_window_group
				 (&hints, parent_window_id);

			 xcb_icccm_wm_hints_set_input(&hints, 0);
		 });
#endif
	auto c=handler->screenref->get_connection()->impl->info->conn;

	xcb_icccm_set_wm_transient_for(c,
				       handler->id(),
				       parent_window_id);

}


popupObj::implObj::implObj(const ref<handlerObj> &handler,
			   const ref<generic_windowObj::handlerObj> &parent)
	: generic_windowObj::implObj(handler),
	  handler(handler)
{
	set_parent_window_of(handler, parent->id());

}

popupObj::implObj::~implObj()
{
	set_parent_window_of(handler, XCB_NONE);
}

LIBCXXW_NAMESPACE_END
