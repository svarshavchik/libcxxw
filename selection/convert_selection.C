/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "selection/current_selection.H"
#include "window_handler.H"
#include "connection_thread.H"
#include "xid_t.H"
#include "catch_exceptions.H"
#include "returned_pointer.H"

LIBCXXW_NAMESPACE_START

void window_handlerObj::paste(IN_THREAD_ONLY, xcb_atom_t clipboard,
			      xcb_timestamp_t timestamp)
{
	paste(IN_THREAD, clipboard, IN_THREAD->info->atoms_info.utf8_string,
	      timestamp);
}

void window_handlerObj::paste(IN_THREAD_ONLY, xcb_atom_t clipboard,
			      xcb_atom_t type, xcb_timestamp_t timestamp)
{
	returned_pointer<xcb_generic_error_t *> error;

	auto conn=IN_THREAD->info->conn;

	auto value=return_pointer(xcb_get_selection_owner_reply
				  (conn, xcb_get_selection_owner(conn,
								 clipboard),
				   error.addressof()));

	if (error)
	{
		LOG_ERROR(connectionObj::implObj::get_error(error));
		return;
	}

	if (value->owner == XCB_NONE)
	{
		LOG_DEBUG(IN_THREAD->info->get_atom_name(type)
			  << " is not available");
		return;
	}

	xcb_delete_property(conn, id(), type);
	incremental_paste=false;
	xcb_convert_selection(conn,
			      id(),
			      clipboard,
			      type,
			      type,
			      timestamp);
}

void window_handlerObj::conversion_failed(IN_THREAD_ONLY, xcb_atom_t clipboard,
					  xcb_atom_t type,
					  xcb_timestamp_t timestamp)
{
	if (type == IN_THREAD->info->atoms_info.utf8_string)
		paste(IN_THREAD, clipboard,
		      IN_THREAD->info->atoms_info.string,
		      timestamp);
}

void window_handlerObj
::selection_notify_event(IN_THREAD_ONLY,
			 const xcb_selection_notify_event_t *msg)
{
	if (msg->property == XCB_NONE)
		conversion_failed(IN_THREAD, msg->selection,
				  msg->target, msg->time);
}

void window_handlerObj
::property_notify_event(IN_THREAD_ONLY,
			const xcb_property_notify_event_t *msg)
{
	if (msg->state != XCB_PROPERTY_NEW_VALUE)
		return;

	LOG_DEBUG("New Property Value: "
		  << IN_THREAD->info->get_atom_name(msg->atom));

	if (msg->atom == IN_THREAD->info->atoms_info.string)
	{
		if (most_recent_paste != msg->atom)
		{
			end();
			begin(unicode::iso_8859_1);
		}
	}
	else if (msg->atom == IN_THREAD->info->atoms_info.utf8_string)
	{
		if (most_recent_paste != msg->atom)
		{
			end();
			begin(unicode::utf_8);
		}
	}
	else
	{
		return;
	}

	bool nonempty_property=false;

	most_recent_paste=msg->atom;
	IN_THREAD->info->get_entire_property_with
		(id(), msg->atom,
		 XCB_GET_PROPERTY_TYPE_ANY, false,
		 [&]
		 (xcb_atom_t type,
		  uint8_t format,
		  void *data,
		  size_t data_size)
		 {
			 if (type == IN_THREAD->info->atoms_info.incr)
				 incremental_paste=true;

			 nonempty_property=true;

			 LOG_DEBUG("Incremental paste: "
				   << incremental_paste
				   << ", size=" << data_size);

			 // Ignore INCR types.
			 if (format != 8 || type != msg->atom)
				 return;

			 unicode::iconvert::tou::operator()
				 (reinterpret_cast<char *>(data), data_size);
		 });

	if (!nonempty_property)
		incremental_paste=false;

	if (!incremental_paste)
	{
		end();
		most_recent_paste=0;
	}
	xcb_delete_property(IN_THREAD->info->conn, id(), msg->atom);
}

int window_handlerObj::converted(const char32_t *ptr, size_t cnt)
{
	LOG_DEBUG("Pasted " << cnt << " characters");
	try {
		// This is called from property_notify_event(), above.
		pasted_string(thread(), {ptr, cnt});
	} CATCH_EXCEPTIONS;
	return 0;
}

void window_handlerObj::pasted_string(IN_THREAD_ONLY,
				      const std::experimental::u32string_view &)
{
}

LIBCXXW_NAMESPACE_END
