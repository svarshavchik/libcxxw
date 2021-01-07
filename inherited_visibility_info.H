/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef inherited_visibility_info_h
#define inherited_visibility_info_h

#include "x/w/namespace.H"

LIBCXXW_NAMESPACE_START

//! Parameter for elementObj::implObj::set_inherited_visibility()

struct LIBCXX_HIDDEN inherited_visibility_info {

	//! The new value for the inherited_visibility_flag

	bool flag;

	//! Do not redraw the display element, when making it visible.

	//! This is used when the visibility changes as a result
	//! of MapWindow(). We expect to receive exposure events, that
	//! will take care of drawing the display elements for us.
	bool do_not_redraw;
};

LIBCXXW_NAMESPACE_END

#endif
