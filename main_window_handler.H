/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef main_window_handler_h
#define main_window_handler_h

#include "generic_window_handler.H"
#include "main_window.H"
#include "connection_threadfwd.H"

LIBCXXW_NAMESPACE_START

//! Common logic for generic input/output windows.

//! Note that this class doesn't handle xcb_create_window and
//! xcb_destroy_window. An implementation class that holds a reference on
//! this handler is responsible for that.

class LIBCXX_HIDDEN main_windowObj::handlerObj :
	public generic_windowObj::handlerObj {

	std::function<void ()> on_delete_callback_thread_only;

 public:

	THREAD_DATA_ONLY(on_delete_callback);

	handlerObj(IN_THREAD_ONLY, const screen &parent_screen);

	//! Destructor
	~handlerObj();

	void client_message_event(IN_THREAD_ONLY,
				  const xcb_client_message_event_t *event)
		override;
};

LIBCXXW_NAMESPACE_END

#endif
