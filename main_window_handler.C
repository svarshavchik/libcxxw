/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "main_window_handler.H"
#include "connection_thread.H"
#include "draw_info.H"

LIBCXXW_NAMESPACE_START

main_windowObj::handlerObj::handlerObj(IN_THREAD_ONLY,
				       const constructor_params &params)
	: generic_windowObj::handlerObj(IN_THREAD, params),
	on_delete_callback_thread_only([] {})
{
	// The top level window is not a child element in a container,
	// so it is, hereby, initialized!
	//
	// We cannot go through the proper channels, i.e. initialize_if_needed()
	// because it calls schedule_update_visibility() in order to
	// kick-start and held visibility changes. However we CANNOT
	// do that, yet. We can't even do this outside of the constructor,
	// since schedule_update_visibility pokes connection thread-only
	// containers, and we are NOT in the connection thread.

	data(IN_THREAD).initialized=true;

	// Set WM_PROTOCOLS to WM_DELETE_WINDOW -- we handle the window
	// close request ourselves.

	xcb_atom_t protocols[1];

	protocols[0]=conn()->atoms_info.wm_delete_window;

	change_property(IN_THREAD,
			XCB_PROP_MODE_REPLACE,
			conn()->atoms_info.wm_protocols,
			XCB_ATOM_ATOM,
			sizeof(xcb_atom_t)*8,
			1,
			protocols);
}

main_windowObj::handlerObj::~handlerObj()=default;

void main_windowObj::handlerObj
::client_message_event(IN_THREAD_ONLY,
		       const xcb_client_message_event_t *event)
{
	if (event->type == IN_THREAD->info->atoms_info.wm_protocols)
	{
		if (event->data.data32[0] ==
		    IN_THREAD->info->atoms_info.wm_delete_window)
		{
			on_delete_callback(IN_THREAD)();
			return;
		}
	}
}

LIBCXXW_NAMESPACE_END
