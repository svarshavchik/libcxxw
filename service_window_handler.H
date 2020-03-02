/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef service_window_handler_h
#define service_window_handler_h

#include "window_handler.H"
#include <vector>
#include <string_view>
#include <xcb/xproto.h>

LIBCXXW_NAMESPACE_START

//! Connect to an X protocol service provider.

//! Generic service search and location. Currently used for locating the
//! X input method service. Generic algorithm that can be used to find any
//! service in the following manner.
//!
//! 1) connect_service() gets called after this object is constructed.
//!
//! 2) Get a property name "service_name" from screen #0's root window,
//! containing a list of atoms.
//!
//! 3) For each atom in the list, if there's a selection owner for the atom,
//! request the selection owner to convert the selection to one or more
//! property names. If all property selection succeed, return the properties
//! as the found service.

class LIBCXX_HIDDEN service_window_handlerObj : public window_handlerObj {

	//! The established server we connect to.
	xcb_window_t owner_thread_only=XCB_NONE;

	//! A queue of servers to try.
	std::vector<xcb_atom_t> server_atoms;

	//! Attribute atoms.
	std::vector<xcb_atom_t> attribute_atoms;

 public:
	THREAD_DATA_ONLY(owner);

	using window_handlerObj::window_handlerObj;

	//! Destructor
	~service_window_handlerObj();

	//! Attempt to establish a connection to a service.

	//! This results in found_server() being called, after locating each
	//! suitable server. After all servers have been searched, and none of
	//! them were acceptable to found_server(), all_servers_tried() gets
	//! called.

	void connect_service(ONLY IN_THREAD,
			     const char *service_name,
			     const char * const * attributes);

 private:

	//! Try next server.

	//! This attempts to check the next server for all of the required
	//! attributes, by converting the attribute_atoms selections.
	//!
	//! If there are no available server, all_servers_tried().

	void connect_next_server(ONLY IN_THREAD);

	//! The server currently being tried.
	xcb_window_t selection_owner;

	//! The attribute being checked next.

	size_t current_attribute_checked;

	//! Check the next attribute from the server.

	//! If all attributes exist, calls found_server(), otherwise
	//! server_failed().

	void check_next_attribute(ONLY IN_THREAD);

 protected:
	//! The values of the attributes.
	std::vector<std::string> attribute_values;

	//! The currently probed server does not support an attribute.

	//! Calls server_failed().
	void conversion_failed(ONLY IN_THREAD,
			       xcb_atom_t type) override;

	//! Received new property.

	bool begin_converted_data(ONLY IN_THREAD, xcb_atom_t type,
				  xcb_timestamp_t timestamp) override;

	//! The currently probed server's attribute.

	void converted_data(ONLY IN_THREAD, xcb_atom_t clipboard,
			    xcb_atom_t actual_type,
			    xcb_atom_t format,
			    void *data,
			    size_t size) override;

	//! End of the currently probed server's attribute.
	void end_converted_data(ONLY IN_THREAD) override;

 private:

	//! The current server has failed.

	void server_failed(ONLY IN_THREAD);

	//! Check of the server's attributes are acceptable.

	//! If true is returned, sets owner, and we're done.
	//! If false is returned, we try to connect_next_server().

	virtual bool found_server(ONLY IN_THREAD, xcb_window_t owner)=0;

	//! All servers have been tried, none were acceptable.

	virtual void all_servers_tried(ONLY IN_THREAD)=0;
};

LIBCXXW_NAMESPACE_END

#endif
