/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "connection_info.H"
#include "connection_thread.H"
#include "window_handler.H"
#include "xid_t.H"
#include <xcb/xproto.h>

LIBCXXW_NAMESPACE_START

void connection_threadObj::run_event(const xcb_generic_event_t *event)
{
	connection_thread thread_(this);

	// Macro: declare a local variable named "msg" which is a
	// reinterpret_cast-ed event parameter, pointing to a xcb_msg_type_t

#define GET_MSG(msg_type)						\
	auto msg=reinterpret_cast<const xcb_ ## msg_type ## _t		\
				  *>(event)

	// Macro: search window_handlers_thread_only for window_field
	// in msg.

#define FIND_HANDLER(window_field) \
	auto iter=window_handlers_thread_only->find(msg->window_field);	\
									\
	if (iter == window_handlers_thread_only->end()) return

	// Macro invoke "callback" of the handler found by FIND_HANDLER(),
	// passing msg as its parameters.

#define DISPATCH_HANDLER(callback, msg)		\
	iter->second->callback msg

	switch (event->response_type & ~0x80) {
	case 0:
		{
			const xcb_generic_error_t *error=
				reinterpret_cast<const xcb_generic_error_t *>
				(event);

			report_error(error);
		}
		return;
	case XCB_DESTROY_NOTIFY:
		{
			GET_MSG(destroy_notify_event);

			recycle_xid(msg->window);
		}
		return;
	case XCB_CLIENT_MESSAGE:
		{
			GET_MSG(client_message_event);

			FIND_HANDLER(window);

			DISPATCH_HANDLER(client_message_event,(IN_THREAD, msg));
		}
		return;
	};
}

void connection_threadObj::recycle_xid(uint32_t xid)
{
	destroyed_xids_thread_only->erase(xid);
}

LIBCXXW_NAMESPACE_END
