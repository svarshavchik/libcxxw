/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "connection_info.H"
#include "connection_thread.H"
#include "window_handler.H"
#include "xid_t.H"
#include "catch_exceptions.H"
#include <xcb/xproto.h>
#include <x/sentry.H>

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
	case XCB_CONFIGURE_NOTIFY:
		{
			GET_MSG(configure_notify_event);
			FIND_HANDLER(window);
			DISPATCH_HANDLER(configure_notify,
					 (IN_THREAD,
					 {
						 msg->x,
						 msg->y,
						 msg->width,
						 msg->height
					 }));
		}
		return;
	case XCB_PROPERTY_NOTIFY:
		{
			GET_MSG(property_notify_event);

			if (msg->window == root_window_thread_only &&
			    msg->atom == info->atoms_info.cxxwtheme)
			{
				try {
					(*cxxwtheme_changed_thread_only)();
				} CATCH_EXCEPTIONS;

				for (const auto &window_handler
					     : *window_handlers_thread_only)
					window_handler.second
						->theme_updated_event
						(IN_THREAD);
			}
		}
		return;
	case XCB_CLIENT_MESSAGE:
		{
			GET_MSG(client_message_event);

			FIND_HANDLER(window);

			DISPATCH_HANDLER(client_message_event,(IN_THREAD, msg));
		}
		return;
	case XCB_EXPOSE:
		{
			GET_MSG(expose_event);

			FIND_HANDLER(window);

			// Note that X and Y coordinates in Exposure are
			// unsigned. Which makes sense; but we're stuffing this
			// into a generic rectangle, which can have negative
			// cordinates; hence rectangle's X & Y are signed.

			exposed_rectangles_thread_only
				->insert({msg->x, msg->y, msg->width,
							msg->height});

			if (msg->count)
				return; // More exposure events coming.

			// Just clear the set of rectangles when we leave this
			// scope.

			auto sentry=
				make_sentry([s=this->exposed_rectangles_thread_only]
					    {
						    s->clear();
					    });

			sentry.guard();
			iter->second->exposure_event
				(IN_THREAD,
				 *exposed_rectangles_thread_only);

		}
		return;
	};
}

void connection_threadObj::recycle_xid(uint32_t xid)
{
	destroyed_xids_thread_only->erase(xid);
}

void connection_threadObj
::dispatch_set_theme_changed_callback(xcb_window_t root_window,
				      const std::function<void ()> &callback)
{
	root_window_thread_only=root_window;
	*cxxwtheme_changed_thread_only=callback;

	values_and_mask change_notify{XCB_CW_EVENT_MASK,
			XCB_EVENT_MASK_PROPERTY_CHANGE};

	xcb_change_window_attributes(info->conn,
				     root_window,
				     change_notify.mask(),
				     change_notify.values().data());
}

LIBCXXW_NAMESPACE_END
