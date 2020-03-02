/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "xim_generic_window.H"
#include "generic_window_handler.H"
#include "xim/ximclient.H"
#include "xim/ximxtransport.H"
#include "xim/ximxtransport_impl.H"
#include "x/w/screen.H"
#include "x/w/connection.H"

LIBCXXW_NAMESPACE_START

xim_generic_windowObj::xim_generic_windowObj(const ref<generic_windowObj
					     ::handlerObj> &handler)
	: xim(handler->screenref->get_connection()->impl->get_ximxtransport
	      (handler->screenref->get_connection())),
	  xim_client(ximclient::create(xim->impl, handler))
{
	xim_client->xim_client_register();
}

xim_generic_windowObj::~xim_generic_windowObj()
{
	xim_client->xim_client_deregister();
}

LIBCXXW_NAMESPACE_END
