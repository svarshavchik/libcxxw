/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef xim_generic_window_h
#define xim_generic_window_h

#include "x/w/generic_windowobj.H"
#include "xim/ximxtransportfwd.H"
#include "xim/ximclientfwd.H"

#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Attach a XIM client to a generic window.

//! The constructor initiates a connection to the XIM server for the given
//! generic_windowObj::handlerObj. The destructor disconnects it.

class LIBCXX_HIDDEN xim_generic_windowObj : virtual public obj {

 public:

	//! The constructor initiates a connection to the XI
	xim_generic_windowObj(const ref<generic_windowObj::handlerObj>
			      &handler);

	~xim_generic_windowObj();

	//! The X Input Method transport
	const ximxtransport xim;

	//! The X Input Method client for the window.
	const ximclient xim_client;
};

LIBCXXW_NAMESPACE_END

#endif
