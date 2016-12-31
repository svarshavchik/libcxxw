/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef connection_h
#define connection_h

#include <vector>

#include "render.H"
#include "ewmh.H"
#include "connection_threadfwd.H"
#include "connection_infofwd.H"
#include "x/w/connection.H"
#include "x/w/screenobj.H"

#include <x/mpobj.H>
#include <xcb/xcb.h>
#include <xcb/render.h>

LIBCXXW_NAMESPACE_START

//! xcb_connection_t owner.

class LIBCXX_HIDDEN connectionObj::implObj : virtual public obj {

 public:

	class threadEventHandlerObj;

	//! The actual connection handle
	const connection_info info;

	//! The connection's thread.
	const connection_thread thread;

	//! Server configuration

	const struct xcb_setup_t setup;

	//! Server configuration - RENDER extension.

	const render render_info;

	//! Server configuration - EWMH extension

	mpobj<ewmh> ewmh_info;

	//! Server configuration - screens

	const std::vector<ref<screenObj::implObj>> screens;

	//! Constructor, connect to the given display.

	implObj(const std::experimental::string_view &display);

 private:

	//! Intenral constructor

	implObj(const connection_info &info);

	//! Internal constructor

	implObj(const connection_info &info,
		const xcb_setup_t *setup,
		const connection_thread &threadArg);

 public:

	//! Wrap everything up, call xcb_disconnect()
	~implObj() noexcept;

	//! Return a brief identifier for a protocol error.

	static std::string get_error(const xcb_generic_error_t *e);

};

LIBCXXW_NAMESPACE_END

#endif
