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

LIBCXXW_NAMESPACE_START

void connection_threadObj::run_event(ONLY IN_THREAD,
				     const xcb_generic_event_t *event)
{
#ifdef CONNECTION_RUN_EVENT
	CONNECTION_RUN_EVENT();
#endif
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

	// Any message other than motion_notify means that we should flush out
	// any motion_notifys that were buffered.

	if ((event->response_type & ~0x80) != XCB_MOTION_NOTIFY)
		process_buffered_motion_event(IN_THREAD);

	switch (event->response_type & ~0x80) {
	case 0:
		{
			const xcb_generic_error_t *error=
				reinterpret_cast<const xcb_generic_error_t *>
				(event);

			report_error(error);
		}
		return;
	case XCB_KEY_PRESS:
		{
			GET_MSG(key_press_event);

			timestamp(IN_THREAD)=msg->time;

			FIND_HANDLER(event);
			DISPATCH_HANDLER(key_press_event,
					 (IN_THREAD,
					  msg,
					  event->full_sequence >> 16));
		}
		return;
	case XCB_KEY_RELEASE:
		{
			GET_MSG(key_release_event);

			timestamp(IN_THREAD)=msg->time;

			FIND_HANDLER(event);
			DISPATCH_HANDLER(key_release_event,
					 (IN_THREAD,
					  msg,
					  event->full_sequence >> 16));
		}
		return;
	case XCB_BUTTON_PRESS:
		{
			GET_MSG(button_press_event);

			timestamp(IN_THREAD)=msg->time;

			FIND_HANDLER(event);

			DISPATCH_HANDLER(button_press_event,(IN_THREAD, msg));
		}
		return;
	case XCB_BUTTON_RELEASE:
		{
			GET_MSG(button_release_event);

			timestamp(IN_THREAD)=msg->time;

			FIND_HANDLER(event);

			DISPATCH_HANDLER(button_release_event,(IN_THREAD, msg));
		}
		return;
	case XCB_MOTION_NOTIFY:
		{
			GET_MSG(motion_notify_event);

			timestamp(IN_THREAD)=msg->time;

			// We'll buffer this, and not take any action for now.
			// But first, see if there's already a buffered
			// motion_notify event that needs to see the light of
			// day.

			if (buffered_motion_event &&
			    buffered_motion_event->event != msg->event)
				process_buffered_motion_event(IN_THREAD);
			buffered_motion_event=*msg;
		}
		return;
	case XCB_ENTER_NOTIFY:
		{
			GET_MSG(enter_notify_event);

			timestamp(IN_THREAD)=msg->time;
			FIND_HANDLER(event);
			DISPATCH_HANDLER(enter_notify_event, (IN_THREAD, msg));
		}
		return;
	case XCB_LEAVE_NOTIFY:
		{
			GET_MSG(leave_notify_event);

			timestamp(IN_THREAD)=msg->time;
			FIND_HANDLER(event);
			DISPATCH_HANDLER(leave_notify_event, (IN_THREAD, msg));
		}
		return;
	case XCB_DESTROY_NOTIFY:
		{
			GET_MSG(destroy_notify_event);

			recycle_xid(msg->window);
		}
		return;
	case XCB_FOCUS_IN:
		{
			GET_MSG(focus_in_event);
			FIND_HANDLER(event);
			DISPATCH_HANDLER(focus_change_event, (IN_THREAD, true));
		}
		return;
	case XCB_FOCUS_OUT:
		{
			GET_MSG(focus_out_event);
			FIND_HANDLER(event);
			DISPATCH_HANDLER(focus_change_event, (IN_THREAD, false));
		}
		return;
	case XCB_MAP_NOTIFY:
		{
			GET_MSG(map_notify_event);
			FIND_HANDLER(window);
			DISPATCH_HANDLER(process_map_notify_event, (IN_THREAD));
		}
		return;
	case XCB_UNMAP_NOTIFY:
		{
			GET_MSG(unmap_notify_event);
			FIND_HANDLER(window);
			DISPATCH_HANDLER(process_unmap_notify_event,
					 (IN_THREAD));
		}
		return;
	case XCB_CONFIGURE_NOTIFY:
		{
			GET_MSG(configure_notify_event);

			FIND_HANDLER(window);

			// We save and buffer the ConfigureNotify event in
			// each window. We buffer them because resizing the
			// window can generate a bunch of these, and we want
			// to delay actually processing them until we drain
			// all the messages from the server. However, since
			// the windows' bit-gravity is NONE, we expect that
			// each ConfigureNotify to a different size is going
			// to generate Exposure. This gets handled in
			// configure_notify_received(), so we have to
			// call it.

			rectangle r{msg->x, msg->y, msg->width, msg->height};
			iter->second->pending_configure_notify_event(IN_THREAD)=
				r;
			iter->second->configure_notify_received(IN_THREAD, r);
		}
		return;
	case XCB_PROPERTY_NOTIFY:
		{
			GET_MSG(property_notify_event);

			timestamp(IN_THREAD)=msg->time;

			// We can be handling several things here.

			// 1) CXXWTHEME in the screen 0's root window.

			if (msg->window == IN_THREAD->root_window(IN_THREAD) &&
			    msg->atom == info->atoms_info.cxxwtheme)
			{
				// The callback loads and parses the new theme.

				try {
					(*cxxwtheme_changed_thread_only)
						(IN_THREAD);
				} CATCH_EXCEPTIONS;

				theme_updated(IN_THREAD);
			}

			// 2) We requested someone to convert a selection
			// for us (paste to us).
			auto iter=window_handlers_thread_only
				->find(msg->window);
			if (iter != window_handlers_thread_only->end())
				iter->second->property_notify_event(IN_THREAD,
								    msg);

			// 3) We are sending someone incremental
			// selection updates (paste from us).
			handle_incremental_update(IN_THREAD, msg);
		}
		return;
	case XCB_SELECTION_CLEAR:
		{
			GET_MSG(selection_clear_event);

			FIND_HANDLER(owner);

			DISPATCH_HANDLER(selection_clear_event,
					 (IN_THREAD, msg->selection,
					  msg->time));
		}
		return;
	case XCB_SELECTION_REQUEST:
		{
			GET_MSG(selection_request_event);

			FIND_HANDLER(owner);

			auto request=*msg;

			if (request.property == XCB_NONE)
				request.property=info->atoms_info.cxxwpaste;

			xcb_selection_notify_event_t reply{};
			DISPATCH_HANDLER(selection_request_event,
					 (IN_THREAD, request, reply));

			xcb_send_event(info->conn, 0,
				       msg->requestor, 0,
				       (const char *)&reply);
		}
		return;
	case XCB_SELECTION_NOTIFY:
		{
			GET_MSG(selection_notify_event);

			FIND_HANDLER(requestor);

			DISPATCH_HANDLER(selection_notify_event,
					 (IN_THREAD, msg));
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

			auto &r=iter->second->exposure_rectangles(IN_THREAD);

			r.rectangles.insert({msg->x, msg->y, msg->width,
						msg->height});
			r.complete=msg->count == 0;
		}
		return;
	case XCB_GRAPHICS_EXPOSURE:
		{
			GET_MSG(graphics_exposure_event);

			FIND_HANDLER(drawable);

			auto &r=iter->second
				->graphics_exposure_rectangles(IN_THREAD);

			r.rectangles.insert({msg->x, msg->y, msg->width,
						msg->height});
			r.complete=msg->count == 0;
		}
		return;


	};
}

void connection_threadObj::theme_updated(ONLY IN_THREAD)
{
	for (const auto &window_handler : *window_handlers_thread_only)
		window_handler.second->theme_updated_event(IN_THREAD);
}

bool connection_threadObj::process_buffered_motion_event(ONLY IN_THREAD)
{
	if (!buffered_motion_event)
		return false;

	// Actually process the motion event. For real, this time.

	auto msg= *buffered_motion_event;
	buffered_motion_event.reset();

	auto iter=window_handlers_thread_only->find(msg.event);
	if (iter == window_handlers_thread_only->end()) return false;

	iter->second->pointer_motion_event(IN_THREAD, &msg);
	return true;
}

void connection_threadObj::recycle_xid(uint32_t xid)
{
	destroyed_xids_thread_only->erase(xid);
}

void connection_threadObj
::dispatch_set_theme_changed_callback(xcb_window_t root_window,
				      const functionref<void (ONLY IN_THREAD)
				      > &callback)
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
