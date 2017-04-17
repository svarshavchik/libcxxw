/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef ximxtransport_h
#define ximxtransport_h

#include "xid_t_fwd.H"
#include "windowobj.H"
#include "xim/ximxtransportfwd.H"
#include "x/w/screenfwd.H"
#include <xcb/xproto.h>

LIBCXXW_NAMESPACE_START

//! X Input Method client interface.

class LIBCXX_HIDDEN ximxtransportObj : virtual public obj {

 public:

	class implObj;

	//! Internal implementation object, an input-only window.
	const ref<implObj> impl;

	//! The owner of the input-only window implementation object.

	const ref<windowObj> window_owner;

	//! Public constructor
	ximxtransportObj(const screen &s);

	//! Internal constructor
	ximxtransportObj(const ref<implObj> &impl);

	//! Destructor
	~ximxtransportObj();
};

LIBCXXW_NAMESPACE_END

#endif
