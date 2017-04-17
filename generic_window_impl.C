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
#include "xim/ximclient.H"
#include "xim/ximxtransport.H"
#include "xim/ximxtransport_impl.H"
#include <x/weakptr.H>
#include <xcb/xcb.h>

LIBCXXW_NAMESPACE_START

generic_windowObj::implObj::implObj(const ref<handlerObj> &handler)
	: windowObj(handler), handler(handler),
	  xim(handler->screenref->get_connection()->impl->get_ximxtransport
	      (handler->screenref->get_connection())),
	  xim_client(ximclient::create(xim->impl, handler))
{
	xim_client->xim_client_register();
}

generic_windowObj::implObj::~implObj()
{
	xim_client->xim_client_deregister();
}

LIBCXXW_NAMESPACE_END
