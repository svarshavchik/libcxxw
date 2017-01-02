/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "impl_connection_thread.H"
#include "main_window.H"
#include "main_window_handler.H"
#include "connection_thread.H"
#include "connection_info.H"
#include "connection.H"
#include "element.H"
#include "screen.H"
#include "screen_depthinfo.H"
#include "connection.H"
#include <xcb/xcb.h>

LIBCXXW_NAMESPACE_START

static inline void configure_new_window(IN_THREAD_ONLY,
					const connection &conn,
					const ref<generic_windowObj::handlerObj>
					&handler)
{
	// Set WM_PROTOCOLS to WM_DELETE_WINDOW -- we handle the window
	// close request ourselves.

	xcb_atom_t protocols[1];

	protocols[0]=conn->impl->info->atoms_info.wm_delete_window;

	handler->change_property(IN_THREAD,
				 XCB_PROP_MODE_REPLACE,
				 conn->impl->info->atoms_info.wm_protocols,
				 XCB_ATOM_ATOM,
				 sizeof(xcb_atom_t)*8,
				 1,
				 protocols);
}

main_windowObj::implObj::implObj(const screen &screenref,
				 const ref<handlerObj> &handler,
				 xcb_window_t parent,
				 const rectangle &initial_position,
				 uint16_t window_class)
	: generic_windowObj
	  ::implObj(screenref, handler, parent,
		    0,
		    initial_position,
		    window_class,
		    screenref->impl->toplevelwindow_pictformat->depth,
		    screenref->impl->toplevelwindow_visual->impl->visual_id,
		    screenref->impl->toplevelwindow_colormap->id(),

		    [conn=screenref->get_connection(), handler]
		    (IN_THREAD_ONLY) {
			    configure_new_window(IN_THREAD, conn, handler);
                    }

	  ),
	handler(handler)
{
}

main_windowObj::implObj::~implObj()=default;

void main_windowObj::implObj::on_delete(const std::function<void ()> &callback)
{
	thread()->run_as(RUN_AS,
			 [handler=this->handler, callback]
			 (IN_THREAD_ONLY)
			 {
				 handler->on_delete_callback(IN_THREAD)
					 =callback;
			 });
}

LIBCXXW_NAMESPACE_END
