/*
** Copyright 2017-2021 Double Precision, Inc.
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

static property::value<hms>
conversion_request_timeout(LIBCXX_NAMESPACE_STR
			   "::w::conversion_request_timeout",
			   hms(0, 0, 10));

static auto compute_conversion_request_timeout()
{
	return tick_clock_t::now()+std::chrono::duration_cast
		<tick_clock_t::duration>
		(std::chrono::seconds
		 (conversion_request_timeout.get().seconds()));
}

LOG_FUNC_SCOPE_DECL(LIBCXXW_NAMESPACE::window_handlerObj::convert,
		    convert_log);

bool window_handlerObj::convert_selection(ONLY IN_THREAD, xcb_atom_t selection,
					  xcb_atom_t property,
					  xcb_atom_t type,
					  xcb_timestamp_t timestamp)
{
	LOG_FUNC_SCOPE(convert_log);

	if (selection_request_expiration(IN_THREAD))
	{
		LOG_ERROR("Conversion request already in progress.");
		return false;
	}

	LOG_DEBUG("Convert "
		  << IN_THREAD->info->get_atom_name(selection) << " to "
		  << IN_THREAD->info->get_atom_name(type) << " using "
		  << IN_THREAD->info->get_atom_name(property));

	returned_pointer<xcb_generic_error_t *> error;

	auto conn=IN_THREAD->info->conn;

	xcb_delete_property(conn, id(), property);

	xcb_convert_selection(conn,
			      id(),
			      selection,
			      type,
			      property,
			      timestamp);

	converting_incrementally=false;
	conversion_type=type;
	conversion_property=property;
	do_this_conversion.reset();
	selection_request_expiration(IN_THREAD)=
		compute_conversion_request_timeout();
	return true;
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

		selection_request_expiration(IN_THREAD).reset();
		conversion_failed(IN_THREAD, conversion_type);
	}
}

void window_handlerObj
::property_notify_event(ONLY IN_THREAD,
			const xcb_property_notify_event_t *msg)
{
	LOG_FUNC_SCOPE(convert_log);

	if (msg->state != XCB_PROPERTY_NEW_VALUE)
		return;

	if (!selection_request_expiration(IN_THREAD))
		return; // Stale message?

	if (msg->atom != conversion_property)
		return;

	LOG_DEBUG("New Property Value: "
		  << IN_THREAD->info->get_atom_name(msg->atom));

	LOG_DEBUG("Property conversion started");

	bool empty_property_data=true;

	IN_THREAD->info->get_entire_property_with
		(id(), msg->atom,
		 XCB_GET_PROPERTY_TYPE_ANY, true,
		 [&, this]
		 (xcb_atom_t type,
		  uint8_t format,
		  void *data,
		  size_t data_size)
		 {
			 // Ok we received something.
			 empty_property_data=false;

			 // Set a flag if what we got was an INCR.

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

				 converting_incrementally=true;
				 return;
			 }

			 if (!do_this_conversion) // First time here.
			 {
				 do_this_conversion=false;
				 conversion_type=type;

				 LOG_TRACE("Start conversion of "
					   << IN_THREAD->info
					   ->get_atom_name(type));

				 // Check the actual type in the first
				 // invocation of this callback.

				 try {
					 *do_this_conversion=
						 begin_converted_data
						 (IN_THREAD, type, msg->time);
				 } CATCH_EXCEPTIONS;

				 if (!*do_this_conversion)
				 {
					 LOG_TRACE("Ignoring conversion");
				 }
			 }

			 LOG_TRACE("Property: size=" << data_size);

			 if (!*do_this_conversion)
				 return;

			 if (conversion_type != type)
			 {
				 LOG_ERROR("Property type changed during "
					   "conversion: " << conversion_type
					   << " to " << type);
				 return;
			 }

			 try {
				 converted_data(IN_THREAD, msg->atom, type,
						format, data, data_size);
			 } CATCH_EXCEPTIONS;
		 });

	LOG_DEBUG("Property conversion complete");

	if (converting_incrementally)
	{
		if (!empty_property_data)
		{
			LOG_TRACE("Resetting for the next chunk");
			selection_request_expiration(IN_THREAD)=
				compute_conversion_request_timeout();
			return;
		}
	}
	LOG_DEBUG("Property conversion finished");
	end_conversion_request(IN_THREAD);
}

void window_handlerObj::end_conversion_request(ONLY IN_THREAD)
{
	LOG_FUNC_SCOPE(convert_log);

	selection_request_expiration(IN_THREAD).reset();

	bool succeeded=do_this_conversion && *do_this_conversion;

	try {
		if (succeeded)
		{
			LOG_DEBUG("Completed conversion of "
				  << IN_THREAD->info
				  ->get_atom_name(conversion_type));
			end_converted_data(IN_THREAD);
		}
		else
		{
			LOG_DEBUG("Failed to convert "
				  << IN_THREAD->info
				  ->get_atom_name(conversion_type));
			conversion_failed(IN_THREAD, conversion_type);
		}
	} CATCH_EXCEPTIONS;
	do_this_conversion.reset();
}

void window_handlerObj::timeout_selection_request(ONLY IN_THREAD, int &poll_for)
{
	LOG_FUNC_SCOPE(convert_log);

	if (!selection_request_expiration(IN_THREAD))
		return;

	auto now=tick_clock_t::now();

	if (*selection_request_expiration(IN_THREAD) <= now)
	{
		LOG_ERROR("Selection conversion request timed out.");
		end_conversion_request(IN_THREAD);
		return;
	}

	connection_threadObj::compute_poll_until
		(now,
		 *selection_request_expiration(IN_THREAD), poll_for);
}


void window_handlerObj
::conversion_failed(ONLY IN_THREAD, xcb_atom_t type)
{
}

bool window_handlerObj
::begin_converted_data(ONLY IN_THREAD, xcb_atom_t type,
		       xcb_timestamp_t timestamp)
{
	return false;
}

void window_handlerObj
::converted_data(ONLY IN_THREAD, xcb_atom_t selection,
		 xcb_atom_t actual_type,
		 xcb_atom_t format,
		 void *data,
		 size_t size)
{
}

void window_handlerObj
::end_converted_data(ONLY IN_THREAD)
{
}

LIBCXXW_NAMESPACE_END
