/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef main_window_h
#define main_window_h

#include "x/w/main_window.H"
#include "generic_window.H"

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN main_windowObj::implObj
	: public generic_windowObj::implObj {

 public:

	//! Constructor
	implObj(const screen &screenref,
		const ref<handlerObj> &handler,
		xcb_window_t parent,
		const rectangle &initial_position,
		uint16_t window_class);

	//! Destructor
	~implObj() noexcept;

	//! The handler for the connection thread events.
	const ref<handlerObj> handler;
};

LIBCXXW_NAMESPACE_END

#endif
