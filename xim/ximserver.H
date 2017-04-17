/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef ximserver_H
#define ximserver_H

#include "connection_threadfwd.H"
#include "xim/ximclientfwd.H"
#include "xim/ximserverfwd.H"
#include "xim/ximrequestfwd.H"
#include "xim/ximencoding.H"
#include <x/obj.H>
#include <x/logger.H>
#include <x/functional.H>

#include <xcb/xproto.h>

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <deque>

LIBCXXW_NAMESPACE_START

class ximserverBase : public ptrrefBase {

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
	virtual void send(IN_THREAD_ONLY, const uint8_t *data, size_t n)=0;

	//! Callback: disconnected from the XIM server.
	virtual void xim_disconnected(IN_THREAD_ONLY)=0;

	//! Callback: connection negotiation to the XIm server is completed.
	virtual void xim_fully_connected(IN_THREAD_ONLY)=0;

public:
	//! Constructor
	ximserverObj();

	//! Destructor
	~ximserverObj();

	//! Start an orderly shutdown process.

	void shutdown(IN_THREAD_ONLY);
 protected:

	//! Process a received message from the XIM server.
	void received(IN_THREAD_ONLY, const uint8_t *data, size_t n);

 private:
	void badmessage(IN_THREAD_ONLY, const char *message)
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

	//! An XIM attribute value

	class attrvalue {
	public:
		uint16_t id;
		std::vector<uint8_t> value;

		attrvalue()=default;
		attrvalue(uint16_t idArg,
			  uint32_t valueArg);
	};

	//! An XIM trigger keypress.
	class triggerkey {
	public:
		uint32_t keysym;
		uint32_t modifier;
		uint32_t mask;
	};

	typedef uint8_t eventbuf_t[32]; // XEVENT in the XIM_FORWARD_EVENT

#include "ximclient.inc.H"


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
		uint32_t find_best_input_style(IN_THREAD_ONLY,
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

	typedef std::function<bool (IN_THREAD_ONLY, const ximserver &)
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

	void add_request(IN_THREAD_ONLY,
			 const ximrequest &req,
			 const send_sync_request_callback_t &callback);

	// Synchronous request queue
	std::deque<std::pair<send_sync_request_callback_t, ximrequest>>
		request_queue;
	void send_next_request(IN_THREAD_ONLY);

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

	void stop(IN_THREAD_ONLY);

 private:

	//! Connection has been terminated
	bool stop_flag=false;

	//! Server fully connected
	bool fully_connected=false;

	//! All active input contexts
	std::unordered_map<xim_ic_t, ximclient> input_contexts;

 public:

	//! Create a new input context for a client.
	void create_client(IN_THREAD_ONLY, const ximclient &client);

 private:

	//! Ready to create a new input context for a client.
	bool attempt_to_create_client(IN_THREAD_ONLY, const ximclient &client);
 public:
	//! Destroy the client's input context.
	void destroy_client(IN_THREAD_ONLY, const ximclient &client);

 private:
	class create_ic_requestObj;
	class destroy_ic_requestObj;

	//! Process a reply to a synchronous message.

	//! Received a reply to a synchronous request. Pop the next message
	//! from the request_queue, invoke the lambda with the request queue's
	//! message; send the next request, if there's another request in the
	//! queue.

	template<typename lambda_type>
		void sync_reply_received(IN_THREAD_ONLY, lambda_type &&lambda)
	{
		do_sync_reply_received(IN_THREAD, make_function<void
				       (const ximrequest &req)>
				       (std::forward<lambda_type>(lambda)));
	}

	//! Process a reply to a synchronous message.

	void do_sync_reply_received(IN_THREAD_ONLY, const function<void
				    (const ximrequest &)> &callback);
};

LIBCXXW_NAMESPACE_END

#endif
