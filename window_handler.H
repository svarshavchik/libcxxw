/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef window_handler_h
#define window_handler_h

#include "xid_t.H"
#include "connection_threadfwd.H"
#include <x/logger.H>
#include <xcb/xcb.h>

LIBCXXW_NAMESPACE_START

//! A window message handler.

//! Provides callbacks that the connection thread invokes, in response
//! to messages.
//!
//! This contains a reference to the connection handler, and the connection
//! handler's shared_data contains references to these handlers. The
//! reference from shared_data is installed in generic_windowObj::implObj's
//! constructor. This circular reference is removed from implObj's
//! destructor.

class LIBCXX_HIDDEN window_handlerObj : public xid_t<xcb_window_t>,
					virtual public obj {

 public:

	LOG_CLASS_SCOPE;

	window_handlerObj(IN_THREAD_ONLY);

	~window_handlerObj() noexcept;

	//! Set a window property.

	void change_property(IN_THREAD_ONLY,
			     uint8_t mode,
			     xcb_atom_t property,
			     xcb_atom_t type,
			     uint8_t format,
			     uint32_t data_len,
			     void *data);

	virtual void client_message_event(IN_THREAD_ONLY,
					  const xcb_client_message_event_t *);
};

LIBCXXW_NAMESPACE_END

#endif
