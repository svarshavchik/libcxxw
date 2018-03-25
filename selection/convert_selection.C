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

LOG_FUNC_SCOPE_DECL(LIBCXXW_NAMESPACE::window_handlerObj::convert,
		    convert_log);

void window_handlerObj::convert_selection(ONLY IN_THREAD, xcb_atom_t clipboard,
					  xcb_atom_t type,
					  xcb_timestamp_t timestamp)
{
	LOG_FUNC_SCOPE(convert_log);

	LOG_DEBUG("Convert "
		  << IN_THREAD->info->get_atom_name(clipboard) << " to "
		  << IN_THREAD->info->get_atom_name(type));

	returned_pointer<xcb_generic_error_t *> error;

	auto conn=IN_THREAD->info->conn;

	auto value=return_pointer(xcb_get_selection_owner_reply
				  (conn, xcb_get_selection_owner(conn,
								 clipboard),
				   error.addressof()));

	if (error)
	{
		LOG_ERROR(connection_error(error));
		conversion_failed(IN_THREAD, clipboard, type, timestamp);
		return;
	}

	if (value->owner == XCB_NONE)
	{
		LOG_DEBUG(IN_THREAD->info->get_atom_name(clipboard)
			  << " is not available");
		conversion_failed(IN_THREAD, clipboard, type, timestamp);
		return;
	}

	xcb_delete_property(conn, id(), type);
	xcb_convert_selection(conn,
			      id(),
			      clipboard,
			      type,
			      type,
			      timestamp);
}

void window_handlerObj
::selection_notify_event(ONLY IN_THREAD,
			 const xcb_selection_notify_event_t *msg)
{
	LOG_FUNC_SCOPE(convert_log);

	if (msg->property == XCB_NONE)
	{
		LOG_DEBUG("Conversion of "
			  << IN_THREAD->info->get_atom_name(msg->selection)
			  << " to "
			  << IN_THREAD->info->get_atom_name(msg->target)
			  << " failed");

		conversion_failed(IN_THREAD, msg->selection,
				  msg->target, msg->time);
	}
}

void window_handlerObj
::property_notify_event(ONLY IN_THREAD,
			const xcb_property_notify_event_t *msg)
{
	LOG_FUNC_SCOPE(convert_log);

	if (msg->state != XCB_PROPERTY_NEW_VALUE)
		return;

	LOG_DEBUG("New Property Value: "
		  << IN_THREAD->info->get_atom_name(msg->atom));

	bool do_conversion=false;

	LOG_DEBUG("Property conversion started");

	try {

		do_conversion=begin_converted_data(IN_THREAD, msg->atom,
						   msg->time);
	} CATCH_EXCEPTIONS;

	if (!do_conversion)
	{
		LOG_TRACE("Ignoring conversion");
		return;
	}

	IN_THREAD->info->get_entire_property_with
		(id(), msg->atom,
		 XCB_GET_PROPERTY_TYPE_ANY, true,
		 [&]
		 (xcb_atom_t type,
		  uint8_t format,
		  void *data,
		  size_t data_size)
		 {
			 LOG_TRACE("Property: size=" << data_size);

			 if (type == IN_THREAD->info->atoms_info.incr)
			 {
				 uint32_t size=0;

				 if (data_size >= sizeof(size))
				 {
					 const char *p=reinterpret_cast<char *>
						 (data);

					 std::copy(p, p+sizeof(size),
						   reinterpret_cast<char *>
						   (&size));
				 }
				 LOG_DEBUG("Incremental conversion, estimated "
					   "size is " << size);

				 try {
					 converting_incrementally
						 (IN_THREAD,
						  msg->atom,
						  msg->time,
						  size);
				 } CATCH_EXCEPTIONS;

				 return;
			 }

			 try {
				 converted_data(IN_THREAD, msg->atom, type,
						format, data, data_size);
			 } CATCH_EXCEPTIONS;
		 });

	LOG_DEBUG("Property conversion complete");

	try {
		end_converted_data(IN_THREAD, msg->atom, msg->time);
	} CATCH_EXCEPTIONS;
}

void window_handlerObj
::conversion_failed(ONLY IN_THREAD, xcb_atom_t clipboard,
		    xcb_atom_t type,
		    xcb_timestamp_t timestamp)
{
}

bool window_handlerObj
::begin_converted_data(ONLY IN_THREAD, xcb_atom_t type,
		       xcb_timestamp_t timestamp)
{
	return false;
}

void window_handlerObj
::converting_incrementally(ONLY IN_THREAD,
			   xcb_atom_t type,
			   xcb_timestamp_t timestamp,
			   uint32_t estimated_size)
{
}

void window_handlerObj
::converted_data(ONLY IN_THREAD, xcb_atom_t clipboard,
		 xcb_atom_t actual_type,
		 xcb_atom_t format,
		 void *data,
		 size_t size)
{
}

void window_handlerObj
::end_converted_data(ONLY IN_THREAD,
		     xcb_atom_t type,
		     xcb_timestamp_t timestamp)
{
}

LIBCXXW_NAMESPACE_END
