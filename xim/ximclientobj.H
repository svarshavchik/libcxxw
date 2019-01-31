/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef ximclientobj_H
#define ximclientobj_H

#include <cstdint>
#include "xim/ximserverfwd.H"
#include "xim/ximclientfwd.H"
#include "window_handler.H"
#include "x/w/rectangle.H"

LIBCXXW_NAMESPACE_START

//! X Input Method client object.

//! An implementation object that owns a window_handler must also construct
//! an ximclient, and passes the window handler, and the server, to the
//! constructor.
//!
//! The implementation object's constructor must call xim_client_register().
//!
//! The implementation object's destructor must call xim_client_deregister().
//!
//! This creates a temporary circular reference:
//!
//! - The ximserver associates the client_window with an input_context_id,
//! and stores it in its input_contexts.
//!
//! - xim_client_register() stores a reference to the ximclient in the
//! client_window, which has a reference to the ximserver
//!
//! The implementation object's destructor's call to xim_client_deregister()
//! breaks the circular references.

class LIBCXX_HIDDEN ximclientObj : virtual public obj {

 public:

	//! This is the server interface
	const ximserver server;

	//! This is our client window.
	const ref<window_handlerObj> client_window;

	//! Constructor

	ximclientObj(const ximserver &server,
		     const ref<window_handlerObj> &client_window);

	//! Destructor
	~ximclientObj();

	//! Must be invoked from the implementation object's constructor.

	//! Sends a create_client() request to the server, and invokes
	//! window_handler's register_xim_client().

	void xim_client_register() noexcept;
	//! Must be invoked from the implementation object's destructor.

	//! Sends a destroy_client() request to the server, and invokes
	//! window_handler's deregister_xim_client().

	void xim_client_deregister() noexcept;

 private:

	//! Server-assigned input_context_id

	//! For server's use only.
	xim_ic_t input_context_id_thread_only=0;

	//! Forward event mask
	uint32_t forward_event_mask_thread_only=0;

	//! Synchronous event mask
	uint32_t synchronous_event_mask_thread_only=0;

 public:

	THREAD_DATA_ONLY(input_context_id);
	THREAD_DATA_ONLY(forward_event_mask);
	THREAD_DATA_ONLY(synchronous_event_mask);

 private:
	//! Most recent cursor location that was reported to us.

	rectangle reported_cursor_position_thread_only;

	//! Most recent cursor location that was used to set the spotLocation.

	rectangle sent_cursor_position_thread_only;

 public:
	THREAD_DATA_ONLY(reported_cursor_position);
	THREAD_DATA_ONLY(sent_cursor_position);

	//! Notify the XIM server of our input location.

	//! Sets reported_cursor_position, if it changed, and schedules
	//! a request to update the server.
	void current_cursor_position(ONLY IN_THREAD, const rectangle &);

	//! Notify the XIM server of our input location again.

	//! By-passes internal optimization that suppresses repeated
	//! notifications of our input location. This is done when the
	//! client window gets repositioned. From its perspective
	//! the cursor position hasn't changed, but we need to notify the
	//! XIM server so that it can update itself. Apparently the XIM
	//! server does not listen for ConfigureNotify messages and does not
	//! notice that the client window has moved.
	void resend_cursor_position(ONLY IN_THREAD);

 private:

	//! Whether we should have input focus now, for X Input Method entry.
	bool reported_focus_thread_only=false;

	//! Most recent focus status that was sent to the XIM server.
	bool sent_focus_thread_only=false;
 public:

	THREAD_DATA_ONLY(reported_focus);
	THREAD_DATA_ONLY(sent_focus);

	//! Notify the XIM server whether or not we should have input focus.

	void focus_state(ONLY IN_THREAD, bool flag);

	//! Forward key event to the XIM server.

	bool forward_key_press_event(ONLY IN_THREAD,
				     const xcb_key_press_event_t &e,
				     uint16_t sequencehi);

	//! Forward key event to the XIM server.

	bool forward_key_release_event(ONLY IN_THREAD,
				       const xcb_key_release_event_t &e,
				       uint16_t sequencehi);
};

LIBCXXW_NAMESPACE_END

#endif
