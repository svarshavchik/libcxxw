/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef ewmh_h
#define ewmh_h

#include <xcb/xcb_ewmh.h>
#include <xcb/xproto.h>
#include "x/w/namespace.H"
#include "x/w/rectangle.H"
#include "x/w/main_windowobj.H"
#include <x/logger.H>
#include <unordered_set>

LIBCXXW_NAMESPACE_START

//! Extended Window Manager Hints.

class LIBCXX_HIDDEN ewmh : public xcb_ewmh_connection_t {

 public:
	LOG_CLASS_SCOPE;

	//! Whether EWMH are available
	bool ewmh_available;

	//! Constructor
	ewmh(xcb_connection_t *);

	//! Destructor
	~ewmh();

	//! Return a given screen's work area.

	//! Returns \c false if EWMH were not available, and ret is unchanged.
	//! Otherwise populates ret with the screen's work area and returns
	//! \c true.

	bool get_workarea(size_t screen_number, rectangle &ret);

	//! Request frame extents from the window manager.

	void request_frame_extents(size_t screen_number,
				   xcb_window_t window_id);

	//! Retrieve the given window's frame size.

	//! Indicates how much extra space the window manager slaps on
	//! each window's borders.
	//!
	//! Returns \c false, and sets left, right, top, and bottom, to
	//! 0 if EWMH not available. Otherwise sets them and returns
	//! \c true.

	bool get_frame_extents(dim_t &left,
			       dim_t &right,
			       dim_t &top,
			       dim_t &bottom,
			       xcb_window_t window_id);

	//! Return _NET_SUPPORTED atoms.

	std::unordered_set<xcb_atom_t> get_supported(size_t screen_number);

	//! Set window manager icon.
	void set_wm_icon(xcb_window_t wid,
			 const std::vector<uint32_t> &raw_data);

	//! Set window type.
	void set_window_type(xcb_window_t wid,
			     const std::string &type);

	//! Set window name
	void set_window_name(xcb_window_t wid,
			     const std::string &name);

	//! Set process id
	void set_window_pid(xcb_window_t wid);

	//! Set user time
	void set_user_time(xcb_window_t wid, xcb_timestamp_t t);

	//! Set user time window
	void set_user_time_window(xcb_window_t wid, xcb_window_t time_wid);

	//! Process an XCB_CLIENT_MESSAGE.

	//! Handles a response to a _NET_WM_PING, and returns true.
	//! Handles a response to a _NET_WM_SYNC_REQUEST, and returns true.
	//! Returns false if the client message is not recognized.

	bool client_message(ONLY IN_THREAD,
			    main_windowObj::handlerObj &handler,
			    const xcb_client_message_event_t *event,
			    xcb_window_t root_window);
};

LIBCXXW_NAMESPACE_END

#endif
