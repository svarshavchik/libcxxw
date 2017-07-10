/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "impl_connection_thread.H"
#include "main_window.H"
#include "main_window_handler.H"
#include "x/w/container.H"
#include "run_as.H"
#include "connection_info.H"
#include "connection.H"
#include "element.H"
#include "screen.H"
#include "screen_depthinfo.H"
#include "connection.H"
#include <xcb/xcb.h>

LIBCXXW_NAMESPACE_START

main_windowObj::implObj::implObj(const ref<handlerObj> &handler,
				 const container &peepholed_container)
	: generic_windowObj::implObj(handler),
	xim_generic_windowObj(handler),
	handler(handler),
	peepholed_container(peepholed_container)
{
}

main_windowObj::implObj::~implObj()=default;

void main_windowObj::implObj::on_delete(const std::function<void ()> &callback)
{
	thread()->run_as([handler=this->handler, callback]
			 (IN_THREAD_ONLY)
			 {
				 handler->on_delete_callback(IN_THREAD)
					 =callback;
			 });
}

LIBCXXW_NAMESPACE_END
