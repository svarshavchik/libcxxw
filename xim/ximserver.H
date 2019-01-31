/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef ximserver_H
#define ximserver_H

#include "x/w/impl/connection_threadfwd.H"
#include "xim/ximclientfwd.H"
#include "xim/ximserverfwd.H"
#include "xim/ximrequestfwd.H"
#include "xim/ximencoding.H"
#include "x/w/types.H"
#include <x/obj.H>
#include <x/logger.H>
#include <x/functionalrefptr.H>

#include <xcb/xproto.h>

#include <string>
#include <vector>
#include <unordered_map>
#include <deque>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN ximserverBase : public ptrref_base {
};

//! An XIM attribute value

class LIBCXX_HIDDEN ximattrvalue {
public:
	uint16_t id;
	std::vector<uint8_t> value;

	ximattrvalue()=default;
	ximattrvalue(uint16_t idArg,
		     uint32_t valueArg);
};

// Talk to the X input method server.

// A subclass implements the underlying transport mechanism.
// The subclass implements send(), and sends the opaque message to the
// X input method server.
//
// The subclass invokes received() to report a message received from the
// X input server.
//
// Implements the protocol layer described in the XIM documentation at
// http://www.x.org/releases/X11R7.6/doc/libX11/specs/XIM/xim.html

class LIBCXX_HIDDEN ximserverObj : virtual public obj {

 protected:
	LOG_CLASS_SCOPE;

 private:
	//! Send an opaque packet to the XIM server.
	virtual void send(ONLY IN_THREAD, const uint8_t *data, size_t n)=0;

	//! Callback: disconnected from the XIM server.
	virtual void xim_disconnected(ONLY IN_THREAD)=0;

	//! Callback: connection negotiation to the XIm server is completed.
	virtual void xim_fully_connected(ONLY IN_THREAD)=0;

public:
	//! Constructor
	ximserverObj();

	//! Destructor
	~ximserverObj();

	//! Start an orderly shutdown process.

	void shutdown(ONLY IN_THREAD);
 protected:

	//! Process a received message from the XIM server.
	void received(ONLY IN_THREAD, const uint8_t *data, size_t n);

 private:
	void badmessage(ONLY IN_THREAD, const char *message)
		__attribute__((noreturn));
 public:

	//! An XIM attribute
	class attr {
	public:
		//! Assigned identifier for this attribute.
		uint16_t id;

		//! Attribute's type. See "Data Type"
		uint16_t type;
		std::string name;
	};

	//! Convenince typedef

	typedef ximattrvalue attrvalue;

	//! An XIM trigger keypress.
	class triggerkey {
	public:
		uint32_t keysym;
		uint32_t modifier;
		uint32_t mask;
	};

	typedef uint8_t eventbuf_t[32]; // XEVENT in the XIM_FORWARD_EVENT

#include "xim/ximclient.inc.H"


 private:

	//! My input_method_id
	xim_im_t input_method_id_thread_only;

	//! My encoding
	ximencoding encoding_thread_only;

	//! Input method attributes.

	//! The attribute names are converted to lowercase.
	std::unordered_map<std::string, attr>
		im_attributes_by_name_thread_only;

	//! Input method attributes.

	//! The names are converted to lowercase.
	std::unordered_map<std::string, attr>
		ic_attributes_by_name_thread_only;

	//! Bitmasks, #define XIM*
	std::vector<uint32_t> input_styles_thread_only;

	//! Default forward event mask for all clients
	uint32_t default_forward_event_mask=0;

	//! Default synchronous event mask for all clients
	uint32_t default_synchronous_event_mask=0;
 public:

	THREAD_DATA_ONLY(input_method_id);
	THREAD_DATA_ONLY(encoding);
	THREAD_DATA_ONLY(im_attributes_by_name);
	THREAD_DATA_ONLY(ic_attributes_by_name);
	THREAD_DATA_ONLY(input_styles);

	// Find the best input style

	// The template maps an input style to an ordinal value,
	// find_best_input_style() returns an input style with the highest
	// ordinal value.

	template<typename lambda_type>
		uint32_t find_best_input_style(ONLY IN_THREAD,
					       lambda_type &&lambda)
	{
		if (input_styles(IN_THREAD).empty())
			return 0;

		auto best_input_style=input_styles(IN_THREAD)[0];
		auto ordinal=lambda(best_input_style);

		for (const auto s:input_styles(IN_THREAD))
		{
			auto s_ordinal=lambda(s);

			if (s_ordinal > ordinal)
			{
				ordinal=s_ordinal;
				best_input_style=s;
			}
		}

		return best_input_style;
	}


 private:

	//! Callbacks used by add_request().

	typedef functionref<bool (ONLY IN_THREAD, const ximserver &)
			    > send_sync_request_callback_t;

	// Add a synchronoous request.

	// The callback gets invoked when any existing synchronous requests
	// have been processed.
	//
	// The functor gets called first, to send the synchronous event to the
	// server. If it returns false, the request is skipped, and processing
	// moves to the next queued up synchronous request. Otherwise, the
	// req's appropriate callback function gets invoked when the response
	// is received.

	void add_request(ONLY IN_THREAD,
			 const ximrequest &req,
			 const send_sync_request_callback_t &callback);

	//! A wrapper for add_request() for requests linked to a client.

	//! Deals with a race condition of some request coming in after
	//! a client has been destroyed. destroy_client() gets called from
	//! xim_client_deregister() which gets invoked from generic_window's
	//! implementation object's destructor. There might be some
	//! events in flight at the same time that might result in request
	//! for the client to come in after its destroy message is sent.
	//!
	//! input_context_id gets cleared to 0 when xim_destroy_ic is set.
	//! the wrapper will check this, and if 0, it returns.
	void add_client_request(ONLY IN_THREAD,
				const ximrequest &req,
				const ximclient &client,
				const functionref<bool (ONLY IN_THREAD,
							const ximserver &,
							const ximclient &)>
				&callback);
	// Synchronous request queue
	std::deque<std::pair<send_sync_request_callback_t, ximrequest>>
		request_queue;
	void send_next_request(ONLY IN_THREAD);

 protected:

	//! Callback from the transport layer, to indicate a connection failure.

	//! This is also called when a fatal error is encountered, and as
	//! part of shutdown().
	//!
	//! This sets the stop_flag, and clears the request_queue and
	//! input_contexts.
	//!
	//! It's important to do this in order to be able to recover from
	//! fatal error situations, since both the request_queue, and
	//! especially input_contexts create temporary circular references
	//! to other objects that own indirect references to this server
	//! object. stop() ensures that all such circular references will
	//! be broken.
	//!
	//! Once stop() is called add_request() quietly throws any new
	//! requests on the floor, so any new circular references won't get
	//! stored anywhere.
	//!
	//! stop() also gets called as part of xim_disconnected() (in
	//! some cases, redundantly, but that's ok), when the connection
	//! is tagged as disconnected at the transport level, and we are
	//! no longer protocol_connected(). After we are not
	//! protocol_connected(), received() will ignore any new messages
	//! too.
	//!
	//! Note that shutdown() does not stop() until the orderly
	//! teardown of the connection (XIM_CLOSE, and XIM_DISCONNECT) has
	//! been completed, so received() will remain in business until
	//! that time.

	void stop(ONLY IN_THREAD);

 private:

	//! Connection has been terminated
	bool stop_flag=false;

	//! Server fully connected
	bool fully_connected=false;

	//! Whether a disconnect has been requested.
	bool shutdown_requested=false;

	//! All active input contexts
	std::unordered_map<xim_ic_t, ximclient> input_contexts;

 public:

	//! Create a new input context for a client.
	void create_client(ONLY IN_THREAD, const ximclient &client);

 private:

	//! Ready to create a new input context for a client.
	bool attempt_to_create_client(ONLY IN_THREAD, const ximclient &client);
 public:
	//! Destroy the client's input context.
	void destroy_client(ONLY IN_THREAD, const ximclient &client);

 private:
	class create_ic_requestObj;
	class destroy_ic_requestObj;
	class sync_requestObj;
	class get_ic_valuesObj;

	//! Diagnostic
	void get_ic_values(ONLY IN_THREAD, const ximclient &client,
			   const std::vector<std::string> &values);

	//! Process a reply to a synchronous message.

	//! Received a reply to a synchronous request. Pop the next message
	//! from the request_queue, invoke the lambda with the request queue's
	//! message; send the next request, if there's another request in the
	//! queue.

	template<typename lambda_type>
		void sync_reply_received(ONLY IN_THREAD, lambda_type &&lambda)
	{
		do_sync_reply_received(IN_THREAD, make_function<void
				       (const ximrequest &req)>
				       (std::forward<lambda_type>(lambda)));
	}

	//! Process a reply to a synchronous message.

	void do_sync_reply_received(ONLY IN_THREAD, const function<void
				    (const ximrequest &)> &callback);

 public:
	//! Report a client's spotLocation
	void set_spot_location(ONLY IN_THREAD, const ximclient &client);

	//! Report a client's spotLocation again

	//! By-passes internal optimization that suppresses repeated
	//! notifications of the spotLocation. This is done when the
	//! client window gets repositioned. From its perspective
	//! the cursor position hasn't changed, but we need to notify the
	//! XIM server so that it can update itself. Apparently the XIM
	//! server does not listen for ConfigureNotify messages and does not
	//! notice that the client window has moved.

	void resend_spot_location(ONLY IN_THREAD, const ximclient &client);

	//! Report a client's focus state

	void focus_state(ONLY IN_THREAD, const ximclient &client, bool flag);

	//! Forward a key press or release event.
	bool forward_key_press_release_event(ONLY IN_THREAD,
					     const ximclient &client,
					     const xcb_key_release_event_t &e,
					     uint16_t sequencehi,
					     uint32_t mask);
};

LIBCXXW_NAMESPACE_END

#endif
