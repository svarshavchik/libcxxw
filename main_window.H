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
	implObj(const ref<handlerObj> &handler);

	//! Destructor
	~implObj();

	//! The handler for the connection thread events.
	const ref<handlerObj> handler;

	//! Install a callback to invoke upon close.

	void on_delete(const std::function<void ()> &callback);
};

LIBCXXW_NAMESPACE_END

#endif
