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
