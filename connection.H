/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef connection_h
#define connection_h

#include <vector>

#include "connectionfwd.H"
#include "render.H"
#include "ewmh.H"
#include "keysyms.H"
#include "connection_threadfwd.H"
#include "connection_infofwd.H"
#include "current_border_implfwd.H"
#include "fonts/cached_fontfwd.H"
#include "fonts/fontcollectionfwd.H"
#include "xim/ximxtransportfwd.H"
#include "x/w/connection.H"
#include "x/w/screenobj.H"

#include <x/mpobj.H>
#include <x/threads/run.H>
#include <xcb/xcb.h>
#include <xcb/render.h>

LIBCXXW_NAMESPACE_START

//! xcb_connection_t owner.

class LIBCXX_HIDDEN connectionObj::implObj : virtual public obj {

 public:

	class threadEventHandlerObj;

	//! The actual connection handle
	const connection_info info;

	//! Server configuration - RENDER extension.

	const render render_info;

	//! Server configuration - EWMH extension

	mpobj<ewmh> ewmh_info;

 private:
	//! Keysyms

	const keysyms keysyms_info_thread_only;

	//! Container for the X Input Method server singleton object.

	typedef mpobj<weakptr<ximxtransportptr>> xim_t;

	//! X Input Method server returned by get_ximxtransport().

	xim_t xim;
 public:
	THREAD_DATA_ONLY(keysyms_info);

	//! Server configuration

	const struct xcb_setup_t setup;

	//! A wrapper for the connection thread.

	//! The constructor starts it. The destructor stop()s it.

	class connection_wrapper : public connection_thread {
	public:

		connection_wrapper(const connection_thread &thread);
		~connection_wrapper();

		using connection_thread::operator->;
		using connection_thread::operator*;

		//! The running connection thread info.
		const runthreadbase running_thread;
	};

	//! The connection's thread.

	const connection_wrapper thread;

	//! Returns true if the invoking thread is the connection thread.
	bool is_connection_thread() const;

	//! Server configuration - screens

	const std::vector<ref<screenObj::implObj>> screens;

	//! Constructor, connect to the given display.

	implObj(const std::string_view &display);

	// A cache of individual fonts currently in use
	mpobj<font_cache_t> font_cache;

	// A cache of sorted font lists, currently in use
	mpobj<sorted_font_cache_t> sorted_font_cache;

 private:

	//! Intenral constructor

	implObj(const connection_info &info);

	//! Internal constructor

	implObj(const connection_info &info,
		const xcb_setup_t *setup,
		const connection_thread &threadArg);

 public:

	//! Wrap everything up, call xcb_disconnect()
	~implObj();

	//! Returns the X Input Method transport object for this connection.

	//! The ximxtransport is a singleton. The first call to
	//! get_ximxtransport() constructs it.
	//!
	//! The public conncetion object gets passed as a
	//! parameter to get_ximxtransport().

	ximxtransport get_ximxtransport(const connection &conn);
};

LIBCXXW_NAMESPACE_END

#endif
