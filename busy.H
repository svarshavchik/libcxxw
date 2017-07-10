/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef busy_h
#define busy_h

#include "x/w/busy.H"
#include "x/w/generic_windowobj.H"
#include "x/w/elementobj.H"

LIBCXXW_NAMESPACE_START

//! Construct a busy handle before invoking a callback.

//! Subclass the \ref busy "busy handle". This gets instantiating before
//! invoking a callback and passing to it the \ref busy "busy" handle.

class LIBCXX_HIDDEN busy_impl : public busy {
 public:

	//! The top level display element.
	const ref<generic_windowObj::handlerObj> w;

	//! Constructor
	busy_impl(elementObj::implObj &i, const connection_thread &thread);

	//! Constructor
	busy_impl(const ref<generic_windowObj::handlerObj> &w,
		  const connection_thread &thread);

	//! Destructor
	~busy_impl();

	//! Acquire the underlying mcguffin, blocking pointer and keyboard
	//! activity
	x::ref<x::obj> get_mcguffin() const;
};

LIBCXXW_NAMESPACE_END

#endif
