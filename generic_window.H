/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef generic_window_h
#define generic_window_h

#include "x/w/generic_window.H"
#include "x/w/screen.H"
#include "x/w/types.H"
#include "x/w/rectangle.H"

#include "values_and_mask.H"
#include "elementimpl.H"
#include <xcb/xcb.h>

LIBCXXW_NAMESPACE_START

//! Generic window internal implemenation object.

//! The constructor calls xcb_create_window(), and installs the
//! handler in the connection thread.
//!
//! The destructor removes the handler from the connection thread, and
//! installs the handler's xid in the connection thread's destroyed_xids,
//! then installs a destructor callback on the handler object. When all
//! references to the handler object go out of scope and it gets destroyed,
//! xcb_destroy_window().

class LIBCXX_HIDDEN generic_windowObj::implObj
	: public elementimplObj {

 public:

	//! Constructor
	implObj(const screen &screenref,
		const ref<handlerObj> &handler,
		xcb_window_t parent,
		const rectangle &initial_position,
		const values_and_mask &vm,
		uint16_t window_class,
		depth_t depth=depth_t(XCB_COPY_FROM_PARENT),
		xcb_visualid_t visual=XCB_COPY_FROM_PARENT);


	//! Destructor
	~implObj() noexcept;

	const ref<handlerObj> handler;

	// Find my screen.
 private:
	const screen screenref;
 public:
	inline screen get_screen() { return screenref; }

	//! The window's screen.
	inline const_screen get_screen() const { return screenref; }

	//! Return the window manager's hints.

	bool get_frame_extents(dim_t &left,
			       dim_t &right,
			       dim_t &top,
			       dim_t &bottom)
		const;
};

LIBCXXW_NAMESPACE_END

#endif
