/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef ximxtransport_impl_h
#define ximxtransport_impl_h

#include "service_window_handler.H"
#include "xim/ximxtransport.H"
#include "xim/ximserver.H"

#include <x/mpobj.H>
#include <vector>

LIBCXXW_NAMESPACE_START

//! X Input Method implementation object.

class LIBCXX_HIDDEN ximxtransportObj::implObj
	: public service_window_handlerObj,
	  public ximserverObj {

	//! Keeps track of the current connection state.

	struct connection_state_s {

		//! True: there's an X protocol connection to the XIM server

		//! Gets set to true when found_server() is called,
		bool x_connected=false;

		//! True: protocol completely connected

		bool protocol_connected=false;

		//! True: disconnection complete.

		//! Gets set to true when:
		//!
		//! - all_servers_tried() has been called.
		//!
		//! - disconnected() has been called.

		bool x_disconnected=false;
	};

	//! Typesafe connection state info
	typedef mpcobj<connection_state_s> connection_state_t;

	//! Current protocol version
	uint32_t proto_major_thread_only=0;

	//! Current protocol version
	uint32_t proto_minor_thread_only=0;

	//! Current protocol version
	uint32_t dividing_size_thread_only=0;

	// Buffer where the message being received is assembled.
	std::vector<uint8_t> buffer;
 public:
	THREAD_DATA_ONLY(proto_major);
	THREAD_DATA_ONLY(proto_minor);
	THREAD_DATA_ONLY(dividing_size);

	//! Current connection state
	connection_state_t connection_state;

	//! Update the connection state

	template<typename lambda>
		void update_connection_state(lambda &&l)
	{
		connection_state_t::lock lock{connection_state};

		l(*lock);

		lock.notify_all();
	}

	//! Returns true if x_connected and not x_disconnected

	bool protocol_connected();

	using service_window_handlerObj::service_window_handlerObj;

	//! Destructor
	~implObj();

	//! Attempt to connect to the X Input Method server.

	void connect(ONLY IN_THREAD);

	//! Protocol disconnection from the server
	void disconnected(ONLY IN_THREAD) override;

	//! Wait until we disconnect from the XIM server in an orderly fashion.

	//! This gets called before destroying this implementation object.

	void wait_until_disconnected();

 private:

	//! service_window_handlerObj has found a server for us.

	bool found_server(ONLY IN_THREAD, xcb_window_t) override;

	//! All servers have been tried, none were acceptable.

	void all_servers_tried(ONLY IN_THREAD) override;

	//! Received a client message

	void client_message_event(ONLY IN_THREAD,
				  const xcb_client_message_event_t *)
		override;

	bool begin_converted_data(ONLY IN_THREAD, xcb_atom_t type,
				  xcb_timestamp_t timestamp)
		override;

	void converted_data(ONLY IN_THREAD, xcb_atom_t clipboard,
			    xcb_atom_t actual_type,
			    xcb_atom_t format,
			    void *data,
			    size_t size) override;


	//! Hooray, we received a message from the XIM server.
	void received_single_message(ONLY IN_THREAD,
				     const uint8_t *data, size_t n);

	//////////////////////////////////////////////////////////////////

	// Inherited from ximserverObj

	//! Send a message.
	void send(ONLY IN_THREAD, const uint8_t *data, size_t n)
		override;

	//! Disconnection. Fatal error, or orderly disconnection.
	void xim_disconnected(ONLY IN_THREAD) override;

	//! All connection negotiation has taken place.
	void xim_fully_connected(ONLY IN_THREAD) override;
};

LIBCXXW_NAMESPACE_END

#endif
