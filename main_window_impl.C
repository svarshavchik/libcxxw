/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "main_window.H"
#include "main_window_handler.H"
#include "connection_thread.H"
#include "connection_info.H"
#include "screen.H"
#include "screen_depthinfo.H"
#include "connection.H"
#include <x/weakptr.H>
#include <xcb/xcb.h>

LIBCXXW_NAMESPACE_START

main_windowObj::implObj::implObj(const screen &screenref,
				 const ref<handlerObj> &handler,
				 xcb_window_t parent,
				 const rectangle &initial_position,
				 uint16_t window_class)
	: generic_windowObj::implObj(screenref, handler, parent,
				     0,
				     initial_position,
				     window_class,
				     screenref->impl->toplevelwindow_pictformat
					     ->depth,
				     screenref->impl->toplevelwindow_visual
				     ->impl->visual_id,
				     screenref->impl->toplevelwindow_colormap
					     ->id()),
	handler(handler)
{
}

main_windowObj::implObj::~implObj() noexcept=default;

LIBCXXW_NAMESPACE_END
