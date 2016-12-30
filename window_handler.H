#ifndef window_handler_h
#define window_handler_h

#include "xid_t.H"
#include "connection_thread.H"
#include <x/logger.H>
#include <xcb/xcb.h>

LIBCXXW_NAMESPACE_START

//! A window message handler.

//! Provides callbacks that the connection thread invokes, in response
//! to messages.
//!
//! Its methods can only be called by the connection thread.
//!
//! This contains a reference to the connection handler, but
//! connection handler's map, that maps xcb_window_ts to these handlers,
//! is stored on the connection handler's execution thread's stack, hence
//! no circular reference.

class LIBCXX_HIDDEN window_handlerObj : public xid_t<xcb_window_t>,
					virtual public obj {

 public:

	LOG_CLASS_SCOPE;

	window_handlerObj(const ref<connectionObj::implObj::threadObj> &thread);

	~window_handlerObj() noexcept;
};

LIBCXXW_NAMESPACE_END

#endif
