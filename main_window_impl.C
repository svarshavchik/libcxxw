/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "impl_connection_thread.H"
#include "main_window.H"
#include "main_window_handler.H"
#include "shared_handler_data.H"
#include "x/w/container.H"
#include "x/w/dialog.H"
#include "run_as.H"
#include "connection_info.H"
#include "connection.H"
#include "x/w/impl/element.H"
#include "screen.H"
#include "screen_depthinfo.H"
#include "connection.H"
#include <xcb/xcb.h>

LIBCXXW_NAMESPACE_START

main_windowObj::implObj::implObj(const main_window_impl_args &args)
	: generic_windowObj::implObj(args.handler),
	xim_generic_windowObj(args.handler),
	handler(args.handler),
	menu_and_app_container(args.menu_and_app_container),
	menubar_container(args.menubar_container),
	app_container(args.app_container)
{
	handler->register_current_main_window();
}

main_windowObj::implObj::~implObj()=default;

void main_windowObj::implObj::on_delete(ONLY IN_THREAD,
					const functionref<void
					(THREAD_CALLBACK,
					 const busy &)> &callback)
{
	handler->on_delete_callback(IN_THREAD)=callback;
}

LIBCXXW_NAMESPACE_END
