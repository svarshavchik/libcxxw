/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "service_window_handler.H"
#include <vector>
#include <xcb/xproto.h>
#include "returned_pointer.H"
#include "connection_thread.H"
#include "screen.H"
#include "assert_or_throw.H"

#include <cstring>
#include <algorithm>

LIBCXXW_NAMESPACE_START

service_window_handlerObj::~service_window_handlerObj()=default;

LOG_FUNC_SCOPE_DECL(LIBCXXW_NAMESPACE::service_window_handlerObj,
		    service_log);

void service_window_handlerObj
::connect_service(ONLY IN_THREAD, const char *service_name,
		  const char * const * attributes)
{
	LOG_FUNC_SCOPE(service_log);

	auto conn=IN_THREAD->info->conn;

	server_atoms.clear();
	attribute_atoms.clear();

	LOG_DEBUG("Locating service " << service_name);

	// Retrieve the server property from screen's root window.

	{
		returned_pointer<xcb_generic_error_t *> error;

		auto value=return_pointer(xcb_intern_atom_reply
					  (conn,
					   xcb_intern_atom(conn, 1,
							   strlen(service_name),
							   service_name),
					   error.addressof()));

		if (error)
		{
			LOG_DEBUG("XIM_SERVERS atom is not defined");
			all_servers_tried(IN_THREAD);
			return;
		}

		if (value->atom == XCB_NONE)
		{
			LOG_DEBUG("XIM_SERVERS atom is XCB_NONE");
			all_servers_tried(IN_THREAD);
			return;
		}

		LOG_DEBUG(service_name << " atom is defined");

		IN_THREAD->info->collect_property_with
			(screenref->impl->xcb_screen->root,
			 value->atom,
			 XCB_ATOM_ATOM,
			 false,
			 [&]
			 (xcb_atom_t type,
			  uint8_t format,
			  void *data,
			  size_t data_size)
			 {
				 auto data_atoms=reinterpret_cast<xcb_atom_t *>
					 (data);

				 server_atoms.insert(server_atoms.end(),
						     data_atoms,
						     data_atoms+data_size/
						     sizeof(xcb_atom_t));
			 });
	}

	size_t i;
	for (i=0; attributes[i]; ++i)
		;
	attribute_atoms.reserve(i);

	for (i=0; attributes[i]; ++i)
	{
		returned_pointer<xcb_generic_error_t *> error;

		auto value=return_pointer(xcb_intern_atom_reply
					  (conn,
					   xcb_intern_atom
					   (conn, 1,
					    strlen(attributes[i]),
					    attributes[i]),
					   error.addressof()));

		if (error)
			return;

		if (value->atom == XCB_NONE)
			return;

		LOG_DEBUG(attributes[i] << " atom is defined");

		attribute_atoms.push_back(value->atom);
	}

	connect_next_server(IN_THREAD);
}

void service_window_handlerObj::connect_next_server(ONLY IN_THREAD)
{
	LOG_FUNC_SCOPE(service_log);

	// Keep pruning server_atoms.

	while (!server_atoms.empty())
	{
		auto server=server_atoms[0];

		LOG_DEBUG("Checking "
			  << IN_THREAD->info->get_atom_name(server));

		returned_pointer<xcb_generic_error_t *> error;

		auto value=return_pointer
			(xcb_get_selection_owner_reply
			 (IN_THREAD->info->conn,
			  xcb_get_selection_owner(IN_THREAD->info->conn,
						  server),
			  error.addressof()));

		if (error || value->owner == XCB_NONE)
			continue; // Nobody claims this server.

		selection_owner=value->owner;

		// Prepare to check all attributes.

		attribute_values.clear();
		attribute_values.resize(attribute_atoms.size());

		current_attribute_checked=0;

		check_next_attribute(IN_THREAD);
		return;
	}

	LOG_DEBUG("No more servers");
	all_servers_tried(IN_THREAD);
}

void service_window_handlerObj::check_next_attribute(ONLY IN_THREAD)
{
	LOG_FUNC_SCOPE(service_log);

	if (current_attribute_checked >= attribute_atoms.size())
	{
		LOG_DEBUG("All attributes have been checked.");

		if (found_server(IN_THREAD, selection_owner))
		{
			LOG_DEBUG("Found server");
			owner(IN_THREAD)=selection_owner;
			return;
		}

		server_failed(IN_THREAD);
		return;
	}

	LOG_DEBUG("Checking attribute "
		  << IN_THREAD->info
		  ->get_atom_name(attribute_atoms[current_attribute_checked]));

	if (!convert_selection(IN_THREAD, server_atoms.at(0),
			       /*
			       Ibus is buggy, and requires the target and
			       target atoms to be the same.
			       */
			       attribute_atoms[current_attribute_checked],
			       attribute_atoms[current_attribute_checked],
			       XCB_CURRENT_TIME))
	{
		LOG_DEBUG("convert_selection() failed");
		server_failed(IN_THREAD);
	}
}

void service_window_handlerObj::conversion_failed(ONLY IN_THREAD,
						  xcb_atom_t type)
{
	LOG_FUNC_SCOPE(service_log);

	LOG_DEBUG(IN_THREAD->info
		  ->get_atom_name(server_atoms.at(0))
		  << " attribute conversion failed");
	server_failed(IN_THREAD);
}

bool service_window_handlerObj
::begin_converted_data(ONLY IN_THREAD, xcb_atom_t type,
		       xcb_timestamp_t timestamp)
{
	return current_attribute_checked < attribute_atoms.size() &&
		type == attribute_atoms.at(current_attribute_checked);
}

void service_window_handlerObj
::converted_data(ONLY IN_THREAD, xcb_atom_t clipboard,
		 xcb_atom_t actual_type,
		 xcb_atom_t format,
		 void *data,
		 size_t size)
{
	if (clipboard == attribute_atoms.at(current_attribute_checked)
	    && actual_type == attribute_atoms.at(current_attribute_checked)
	    && format == 8)
	{
		auto s=reinterpret_cast<char *>(data);

		auto &v=attribute_values.at(current_attribute_checked);
		v.insert(v.end(), s, s+size);
	}
}

void service_window_handlerObj
::end_converted_data(ONLY IN_THREAD)
{
	LOG_FUNC_SCOPE(service_log);

	LOG_DEBUG(IN_THREAD->info
		  ->get_atom_name(attribute_atoms.at
				  (current_attribute_checked))
		  << " attribute converted: "
		  << attribute_values.at(current_attribute_checked));
	++current_attribute_checked;
	check_next_attribute(IN_THREAD);
}

void service_window_handlerObj::server_failed(ONLY IN_THREAD)
{
	LOG_FUNC_SCOPE(service_log);

	assert_or_throw(!server_atoms.empty(),
			"Internal error: server_atoms is empty().");

	server_atoms.erase(server_atoms.begin());
	connect_next_server(IN_THREAD);
}

LIBCXXW_NAMESPACE_END
