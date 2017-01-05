/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "main_window_handler.H"
#include "connection_thread.H"

LIBCXXW_NAMESPACE_START

main_windowObj::handlerObj::handlerObj(IN_THREAD_ONLY,
				       const constructor_params &params)
	: generic_windowObj::handlerObj(IN_THREAD, params),
	on_delete_callback_thread_only([] {})
{
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
