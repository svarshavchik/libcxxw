#ifndef ewmh_h
#define ewmh_h

#include <xcb/xcb_ewmh.h>
#include "x/w/namespace.H"
#include "x/w/rectangle.H"
#include <x/logger.H>

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
			       size_t screen_number,
			       xcb_window_t window_id);

 private:

	//! \internal
	bool get_frame_extents(dim_t &left,
			       dim_t &right,
			       dim_t &top,
			       dim_t &bottom,
			       xcb_window_t window_id);
};

LIBCXXW_NAMESPACE_END

#endif
