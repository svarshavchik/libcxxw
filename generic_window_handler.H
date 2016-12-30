#ifndef generic_window_handler_h
#define generic_window_handler_h

#include "window_handler.H"
#include "generic_window.H"

LIBCXXW_NAMESPACE_START

//! Common logic for generic input/output windows.

//! Note that this class doesn't handle xcb_create_window and
//! xcb_destroy_window. An implementation class that holds a reference on
//! this handler is responsible for that.

class LIBCXX_HIDDEN generic_windowObj::handlerObj :
	public window_handlerObj::window_handlerObj {

 public:

	using window_handlerObj::window_handlerObj;

	//! Destructor
	~handlerObj() noexcept;
};

LIBCXXW_NAMESPACE_END

#endif
