/*
** Copyright 2017-2021 Double Precision, Inc.
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

class busy_impl : public busy {
public:

	//! The top level display element.
	const ref<generic_windowObj::handlerObj> w;

	//! Constructor
	busy_impl(elementObj::implObj &i);

	//! Constructor
	busy_impl(const ref<generic_windowObj::handlerObj> &w);

	//! Destructor
	~busy_impl();

	//! Acquire a busy mcguffin that draws a shade.

	ref<obj> get_shade_busy_mcguffin() const override;

	//! Acquire a busy mcguffin that puts up a "wait" cursor.

	ref<obj> get_wait_busy_mcguffin() const override;
};

LIBCXXW_NAMESPACE_END

#endif
