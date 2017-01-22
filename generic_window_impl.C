/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "generic_window.H"
#include "generic_window_handler.H"
#include "connection_thread.H"
#include "connection_info.H"
#include "impl_connection_thread.H"
#include "screen.H"
#include "connection.H"
#include <x/weakptr.H>
#include <xcb/xcb.h>

LIBCXXW_NAMESPACE_START

generic_windowObj::implObj::implObj(const ref<handlerObj> &handler)
	: handler(handler)
{
	LOG_DEBUG("Constructor: " << objname() << " xid " << handler->id());

	thread()->run_as(RUN_AS,
			 [handler]
			 (IN_THREAD_ONLY)
			 {
				 IN_THREAD->window_handlers(IN_THREAD)
					 ->insert({handler->id(), handler});
			 });
}

generic_windowObj::implObj::~implObj()
{
	auto window_id=handler->id();

	LOG_DEBUG("Destructor: " << objname() << " xid " << window_id);

	// Disconnect the handler from the connection thread's window handler
	// map.

	thread()->run_as(RUN_AS,
			 [handler=this->handler, window_id]
			 (IN_THREAD_ONLY)
			 {
				 IN_THREAD->window_handlers(IN_THREAD)
					 ->erase(window_id);
				 IN_THREAD->destroyed_xids(IN_THREAD)
					 ->insert({window_id,handler->xid_obj});
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
				 (RUN_AS,
				  [window_id, screen]
				  (IN_THREAD_ONLY)
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
