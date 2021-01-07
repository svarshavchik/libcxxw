/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "windowobj.H"
#include "window_handler.H"
#include "screen.H"
#include "connection_thread.H"
#include "xid_t.H"

LOG_CLASS_INIT(LIBCXXW_NAMESPACE::windowObj);

LIBCXXW_NAMESPACE_START

windowObj::windowObj(const ref<window_handlerObj> &handler)
	: handler(handler)
{
	LOG_DEBUG("Constructor: " << objname() << " xid " << handler->id());

	handler->thread()->run_as
		([handler]
		 (ONLY IN_THREAD)
		 {
			 IN_THREAD->install_window_handler(IN_THREAD, handler);
		 });
}

windowObj::~windowObj()
{
	auto window_id=handler->id();

	LOG_DEBUG("Destructor: " << objname() << " xid " << window_id);

	// Disconnect the handler from the connection thread's window handler
	// map.

	handler->thread()->run_as
		([handler=this->handler]
		 (ONLY IN_THREAD)
		 {
			 IN_THREAD->uninstall_window_handler(IN_THREAD, handler);
		 });

	// Attach a destructor callback to the handler object.

	// We need to flush the X protocol buffer, so that the display
	// server gets the DestroyWindow request, and sends a DestroyNotify.
	// We do that by scheduling a run_as() job. This guarantees that
	// the connection thread will call xcb_flush().
	//
	// handler object's destructor calls xcb_destroy_window(), and
	// the xcb_flush() has to happen after that.
	//
	// Note that there's a handler reference in window_handlers,
	// so the following can't happen until the above run_as() completes,
	// and until all other references to the handler go away. It is now
	// safe to execute xcb_destroy_window().
	//
	// Capturing screenref by value is intentional, so that it won't
	// get destroyed until the handler gets destroyed, taking the
	// colormap dud with it...

	handler->ondestroy
		([thread=handler->thread(), window_id,
		  screen=handler->screenref]
		 {
			 LOG_DEBUG("Destroying: xid " << window_id);
			 thread->run_as
				 ([window_id, screen]
				  (ONLY IN_THREAD)
				  {
				  });
		 });

	// Also keep a ref on the screen object until the thread receives
	// DestroyNotify, and drops the xid_obj.

	handler->xid_obj->ondestroy([screen=handler->screenref]
				    {
				    });
}

LIBCXXW_NAMESPACE_END
