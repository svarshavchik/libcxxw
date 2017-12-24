/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "xim/ximxtransport_impl.H"
#include "connection_thread.H"
#include "catch_exceptions.H"
#include <x/strtok.H>
#include <x/property_value.H>
#include <string>
#include <list>
#include <algorithm>
#include <X11/X.h>
#include <X11/Xatom.h>

LIBCXXW_NAMESPACE_START

LOG_FUNC_SCOPE_DECL(LIBCXXW_NAMESPACE::ximxtransportObj,
		    transport_log);
static property::value<bool>
disable_xim(LIBCXX_NAMESPACE_STR "::w::disable_xim", false);

static property::value<bool>
skip_xim_disconnect(LIBCXX_NAMESPACE_STR "::w::skip_xim_disconnect", false);

ximxtransportObj::implObj::~implObj()=default;

void ximxtransportObj::implObj::disconnected(IN_THREAD_ONLY)
{
	xim_disconnected(IN_THREAD);
	service_window_handlerObj::disconnected(IN_THREAD);
}

void ximxtransportObj::implObj::connect(IN_THREAD_ONLY)
{
	if (disable_xim.get())
	{
		LOG_DEBUG("Disabling XIM");
		xim_disconnected(IN_THREAD);
		return;
	}

	const char * const attributes[]={"TRANSPORT", "LOCALES", nullptr};
	connect_service(IN_THREAD, "XIM_SERVERS", attributes);
}

bool ximxtransportObj::implObj::found_server(IN_THREAD_ONLY, xcb_window_t owner)
{
	LOG_FUNC_SCOPE(transport_log);
	// Check TRANSPORT for:
	//
	// @transport=X/

	std::list<std::string> transport_list;

	// Looks like TRANSPORT is a semicolon list:

	strtok_str(attribute_values.at(0), ";", transport_list);

	for (const auto &word:transport_list)
	{
		auto p=std::find(word.begin(), word.end(), '=');

		std::string n{word.begin(), p};

		// The spec says "transport", I see "@transport"

		if (n == "@transport" || n == "transport")
		{
			if (p != word.end())
				++p;

			// And looks like this is a comma-separated list.

			std::list<std::string> list2;

			strtok_str(std::string{p, word.end()}, ",", list2);

			if (std::find(list2.begin(), list2.end(), "X/")
			    != list2.end())
			{
				update_connection_state
					([]
					 (auto &state)
					 {
						 state.x_connected=true;
					 });

				LOG_DEBUG("Connected, sending XCONNECT");

				xcb_client_message_event_t message{};

				message.response_type=XCB_CLIENT_MESSAGE;
				message.format=32;
				message.window=owner;
				message.type=IN_THREAD->info->atoms_info
					.xim_xconnect;
				message.data.data32[0]=id();
				message.data.data32[1]=0;
				message.data.data32[2]=0;

				xcb_send_event(IN_THREAD->info->conn, 0,
					       owner, 0,
					       reinterpret_cast<const char *>
					       (&message));
				return true;
			}
		}
	}

	return false;
}

bool ximxtransportObj::implObj::protocol_connected()
{
	connection_state_t::lock lock{connection_state};

	return lock->x_connected && !lock->x_disconnected;
}

void ximxtransportObj::implObj::all_servers_tried(IN_THREAD_ONLY)
{
	update_connection_state([]
				(auto &state)
				{
					state.x_disconnected=true;
				});
	stop(IN_THREAD);
}

void ximxtransportObj::implObj::xim_disconnected(IN_THREAD_ONLY)
{
	LOG_FUNC_SCOPE(transport_log);

	LOG_DEBUG("Disconnected from the server");
	all_servers_tried(IN_THREAD);
}

void ximxtransportObj::implObj::xim_fully_connected(IN_THREAD_ONLY)
{
	LOG_FUNC_SCOPE(transport_log);

	update_connection_state([]
				(auto &state)
				{
					state.protocol_connected=true;
				});

	LOG_DEBUG("Connection to the server is complete");
}

void ximxtransportObj::implObj
::wait_until_disconnected(bool in_helper_thread)
{
	LOG_FUNC_SCOPE(transport_log);

	LOG_DEBUG("Disconnecting"
		  << (in_helper_thread ? " (using a helper thread)":""));

	connection_state_t::lock lock{connection_state};

	thread()->run_as
		([me=ref<implObj>(this), &logger]
		 (IN_THREAD_ONLY)
		 {
			 if (skip_xim_disconnect.get())
			 {
				 LOG_DEBUG("Disabling XIM orderly shutdown");
				 me->xim_disconnected(IN_THREAD);
				 return;
			 }
			 me->shutdown(IN_THREAD);
		 });

	LOG_DEBUG("Waiting for XIM Server disconnection.");

	lock.wait_for(std::chrono::seconds(30),
		      [&]
		      {
			      return lock->x_disconnected;
		      });

	if (!lock->x_disconnected)
	{
		LOG_FATAL("XIM server not responding, aborting connection.");

		// Something is wrong. Time for dire measures.

		thread()->run_as
			([me=ref<implObj>(this)]
			 (IN_THREAD_ONLY)
			 {
				 me->xim_disconnected(IN_THREAD);
			 });

		lock.wait([&]
			  {
				  return lock->x_disconnected;
			  });
	}
}

void ximxtransportObj::implObj
::client_message_event(IN_THREAD_ONLY,
		       const xcb_client_message_event_t *event)
{
	LOG_FUNC_SCOPE(transport_log);

	service_window_handlerObj::client_message_event(IN_THREAD, event);

	if (!protocol_connected())
		return;

	if (event->type == IN_THREAD->info->atoms_info.xim_xconnect &&
	    event->format == 32)
	{
		owner(IN_THREAD)=event->data.data32[0];
		proto_major(IN_THREAD)=event->data.data32[1];
		proto_minor(IN_THREAD)=event->data.data32[2];
		dividing_size(IN_THREAD)=event->data.data32[3];

		LOG_DEBUG("XCONNECT received: protocol "
			  << proto_major(IN_THREAD) << "." << proto_minor(IN_THREAD)
			  << ", maximum message length="
			  << dividing_size(IN_THREAD));

		xim_connect_send(IN_THREAD, 1, 0, std::vector<std::string>());
		return;
	}

	if (event->type == IN_THREAD->info->atoms_info.xim_moredata &&
	    event->format == 8)
	{
		// Multi-CM
		// Add the message's data to buffer, and wait for more.
		LOG_TRACE("_XIM_MOREDATA message received");
		buffer.insert(buffer.end(), event->data.data8,
			      event->data.data8+20);
		return;
	}

	if (event->type == IN_THREAD->info->atoms_info.xim_protocol && event->format == 8)
	{
		// only-CM, last CM in a Multi-CM.
		//
		// Add the message's data to buffer, invoke received().

		LOG_TRACE("_XIM_PROTOCOL message received");

		buffer.insert(buffer.end(), event->data.data8,
			      event->data.data8+20);
		received_single_message(IN_THREAD, &buffer[0], buffer.size());
		buffer.clear();
		return;
	}

	if (event->type == IN_THREAD->info->atoms_info.xim_protocol &&
	    event->format == 32)
	{
		LOG_TRACE("_XIM_PROTOCOL message received, for atom "
			  << IN_THREAD->info->get_atom_name
			  (event->data.data32[1]));

		// Property-with-CM notification. Read the property, and
		// process it.

		IN_THREAD->info->get_entire_property_with
			(id(),
			 event->data.data32[1],
			 XCB_GET_PROPERTY_TYPE_ANY,
			 true,
			 [=]
			 (xcb_atom_t type,
			  uint8_t format,
			  void *data,
			  size_t data_size)
			 {
				 converted_data(IN_THREAD,
						IN_THREAD->info
						->atoms_info.xim_clientXXX,
						type,
						format,
						data,
						data_size);
			 });
		return;
	}
}

bool ximxtransportObj::implObj
::begin_converted_data(IN_THREAD_ONLY, xcb_atom_t type,
			  xcb_timestamp_t timestamp)
{
	if (type == IN_THREAD->info->atoms_info.xim_clientXXX)
		return true;

	return service_window_handlerObj::begin_converted_data(IN_THREAD,
							       type,
							       timestamp);
}

void ximxtransportObj::implObj
::converted_data(IN_THREAD_ONLY, xcb_atom_t clipboard,
		 xcb_atom_t actual_type,
		 xcb_atom_t format,
		 void *data,
		 size_t size)
{
	if (clipboard != IN_THREAD->info->atoms_info.xim_clientXXX)
	{
		return service_window_handlerObj::converted_data
			(IN_THREAD,
			 clipboard,
			 actual_type,
			 format,
			 data,
			 size);
	}

	auto p=reinterpret_cast<char *>(data);

	buffer.insert(buffer.end(), p, p+size);

	// Figure out the size of the message by peeking at the header

	while (buffer.size() >= 4)
	{
		uint32_t length=((((uint32_t)buffer[2]) << 8)
				 | buffer[3])*4 + 4;

		// Wait until we've received the entire message.
		if (buffer.size() < length)
			break;

		received_single_message(IN_THREAD, &buffer[0], length);

		// Remove the message from the buffer. There could be another
		// message there, so try again.
		buffer.erase(buffer.begin(), buffer.begin()+length);
	}
}

void ximxtransportObj::implObj
::received_single_message(IN_THREAD_ONLY,
			  const uint8_t *data, size_t n)
{
	LOG_DEBUG("Received single message");
	if (!protocol_connected())
		// After all this work, all for nothing...
		return;

	try {
		// Invoke ximserverObj.
		received(IN_THREAD, data, n);
		return;
	} CATCH_EXCEPTIONS;

	// If an exception was thrown prior to the connection
	// being fully opened, consider it broken.

	update_connection_state
		([]
		 (auto &state)
		 {
			 state.x_disconnected=true;
		 });
	stop(IN_THREAD);
}

void ximxtransportObj::implObj
::send(IN_THREAD_ONLY, const uint8_t *data, size_t n)
{
	LOG_FUNC_SCOPE(transport_log);

	if (!protocol_connected())
		return;

	auto conn=IN_THREAD->info->conn;

	if (proto_major(IN_THREAD) != 1 && n <= dividing_size(IN_THREAD))
	{
		LOG_TRACE("send: sending XCB_CLIENT_MESSAGE(_XIM_PROTOCOL)");
		xcb_client_message_event_t message{};
		// CM can be used here.

		message.response_type=XCB_CLIENT_MESSAGE;
		message.format=8;
		message.window=owner(IN_THREAD);
		message.type=IN_THREAD->info->atoms_info.xim_protocol;
		std::copy(data, data+n, &message.data.data8[0]);

		xcb_send_event(conn, 0, owner(IN_THREAD), 0,
			       reinterpret_cast<const char *>(&message));
		return;
	}

	// We can send the message via a property in any protocol except
	// major 0, minor 1.

	if (proto_major(IN_THREAD) > 0 || proto_minor(IN_THREAD) != 1)
	{
		LOG_TRACE("send: setting _clientXXX property");
		xcb_change_property(conn, XCB_PROP_MODE_APPEND,
				    owner(IN_THREAD),
				    IN_THREAD->info->atoms_info
				    .xim_clientXXX,
				    XA_STRING,
				    8,
				    n,
				    reinterpret_cast<const void *>(data));

		// How we notify the server, that depends on the protocol.

		if (proto_major(IN_THREAD) > 0)
		{
			LOG_TRACE("send: notifying via PROPERTY_NOTIFY");
			// Via PropertyNotify message.

			xcb_property_notify_event_t notify{};

			notify.response_type=XCB_PROPERTY_NOTIFY;
			notify.window=owner(IN_THREAD);
			notify.atom=IN_THREAD->info->atoms_info
				.xim_clientXXX;
			notify.time=XCB_CURRENT_TIME;
			notify.state=XCB_PROPERTY_NEW_VALUE;
			xcb_send_event(conn, 0, owner(IN_THREAD), 0,
				       reinterpret_cast<const char *>(&notify)
				       );
		}
		else
		{
			LOG_TRACE("send: sendevent(_clientXXX)");
			// Via client message.
			xcb_client_message_event_t message{};

			message.response_type=XCB_CLIENT_MESSAGE;
			message.format=32;
			message.window=owner(IN_THREAD);
			message.type=IN_THREAD->info
				->atoms_info.xim_protocol;
			message.data.data32[0]=n;
			message.data.data32[1]=IN_THREAD->info
				->atoms_info.xim_clientXXX;

			xcb_send_event(conn, 0, owner(IN_THREAD), 0,
				       reinterpret_cast<const char *>(&message)
				       );
		}
		return;
	}

	// MultiCM

	while (n)
	{
		auto todo=n;

		if (todo > dividing_size(IN_THREAD))
			todo=dividing_size(IN_THREAD);

		xcb_client_message_event_t message=xcb_client_message_event_t();

		message.response_type=XCB_CLIENT_MESSAGE;
		message.format=8;
		message.window=owner(IN_THREAD);
		message.type=n-todo ? IN_THREAD->info->atoms_info
			.xim_moredata:IN_THREAD->info->atoms_info
			.xim_protocol;
		LOG_TRACE("send: sending multiCM ("
			  << IN_THREAD->info->get_atom_name(message.type)
			  << ")");
		std::copy(data, data+todo, &message.data.data8[0]);

		data += todo;
		n -= todo;
		xcb_send_event(conn, 0, owner(IN_THREAD), 0,
			       reinterpret_cast<const char *>(&message));
	}
}

LIBCXXW_NAMESPACE_END
