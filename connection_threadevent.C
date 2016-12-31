/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "connection_info.H"
#include "connection_thread.H"
#include "xid_t.H"
#include <xcb/xproto.h>

LIBCXXW_NAMESPACE_START

void connection_threadObj::run_event(const xcb_generic_event_t *event)
{
#define GET_MSG(msg_type)						\
	auto msg=reinterpret_cast<const xcb_ ## msg_type ## _t		\
				  *>(event);

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
	};
}

void connection_threadObj::recycle_xid(uint32_t xid)
{
	destroyed_xids_thread_only->erase(xid);
}

LIBCXXW_NAMESPACE_END
