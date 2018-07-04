/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef cursor_pointerobj_h
#define cursor_pointerobj_h

#include "x/w/impl/cursor_pointerfwd.H"
#include "iconfwd.H"
#include "x/w/defaultthemefwd.H"
#include "xid_t.H"
#include "x/w/connection_threadfwd.H"
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see cursor_pointer

class LIBCXX_HIDDEN cursor_pointerObj : public xidObj {

public:
	//! The icon the cursor_pointer was created from.

	const icon cursor_pointer_icon;

	//! Constructor
	cursor_pointerObj(const icon &cursor_pointer_icon) LIBCXX_HIDDEN;

	//! Destructor
	~cursor_pointerObj() LIBCXX_HIDDEN;

	//! My ID.

	inline xcb_cursor_t cursor_id() const { return id_; }

	//! Initialize the cursor pointer in the connection thread.

	cursor_pointer initialize(ONLY IN_THREAD);

	//! Theme has been updated.

	cursor_pointer theme_updated(ONLY IN_THREAD,
				     const defaulttheme &new_theme);
};

LIBCXXW_NAMESPACE_END

#endif
