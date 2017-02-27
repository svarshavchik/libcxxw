/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef elementfwd_h
#define elementfwd_h

#include <x/ptrfwd.H>
#include "x/w/elementfwd.H"
#include "x/w/elementobj.H"
#include "x/w/namespace.H"

LIBCXXW_NAMESPACE_START

typedef ref<elementObj::implObj> elementimpl;

typedef ptr<elementObj::implObj> elementimplptr;


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
