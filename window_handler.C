/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "window_handler.H"
#include "connection_thread.H"

LOG_CLASS_INIT(LIBCXX_NAMESPACE::w::window_handlerObj);

LIBCXXW_NAMESPACE_START

window_handlerObj
::window_handlerObj(IN_THREAD_ONLY,
		    const constructor_params &params)
	: xid_t<xcb_window_t>(IN_THREAD),
	screenref(params.screenref)
{
	auto width=params.initial_position.width;
	auto height=params.initial_position.height;

	// We can logically attempt to create a window with zero width or
	// height (empty container, for example). X will complain about
	// BadValue, so turn this into a tiny 1x1 window.

	if (width == 0 || height == 0)
		width=height=1;

	if (width == width.infinite() ||
	    height == height.infinite())
		throw EXCEPTION("Internal error, invalid initial display element size");

	xcb_create_window(conn()->conn,
			  (depth_t::value_type)params.depth,
			  id(),
			  params.parent,
			  (coord_t::value_type)params.initial_position.x,
			  (coord_t::value_type)params.initial_position.y,
			  (dim_t::value_type)width,
			  (dim_t::value_type)height,
			  2, // Border width
			  params.window_class,
			  params.visual,
			  params.events_and_mask.mask(),
			  params.events_and_mask.values().data());
}

window_handlerObj::~window_handlerObj()
{
	xcb_destroy_window(conn()->conn, id());
}


void window_handlerObj::change_property(IN_THREAD_ONLY,
					uint8_t mode,
					xcb_atom_t property,
					xcb_atom_t type,
					uint8_t format,
					uint32_t data_len,
					void *data)
{
	xcb_change_property(IN_THREAD->info->conn, mode, id(), property,
			    type, format, data_len, data);
}

void window_handlerObj::configure_notify(IN_THREAD_ONLY,
					 const rectangle &)
{
}

void window_handlerObj::client_message_event(IN_THREAD_ONLY,
					     const xcb_client_message_event_t *)
{
}

void window_handlerObj::exposure_event(IN_THREAD_ONLY,
				       rectangle_set &)
{
}

void window_handlerObj::theme_updated_event(IN_THREAD_ONLY)
{
}

void window_handlerObj::key_press_event(IN_THREAD_ONLY,
					const xcb_key_press_event_t *event,
					uint16_t sequencehi)
{
}

void window_handlerObj::key_release_event(IN_THREAD_ONLY,
					  const xcb_key_release_event_t *event,
					  uint16_t sequencehi)
{
}

LIBCXXW_NAMESPACE_END
