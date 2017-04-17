/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef ximclientobj_H
#define ximclientobj_H

#include <cstdint>
#include "xim/ximserverfwd.H"
#include "xim/ximclientfwd.H"
#include "window_handler.H"

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
 public:

	THREAD_DATA_ONLY(input_context_id);
};

LIBCXXW_NAMESPACE_END

#endif
