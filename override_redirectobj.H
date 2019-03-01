/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_override_redirectobj_h
#define x_w_override_redirectobj_h

#include "override_redirectfwd.H"

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see override_redirect

class override_redirectObj: virtual public obj {

public:
	//! Constructor
	override_redirectObj();

	//! Destructor
	~override_redirectObj();

	//! Return my mixed-in handler object.
	virtual generic_windowObj::handlerObj &
	get_override_redirected_window_handler()=0;
};

LIBCXXW_NAMESPACE_END

#endif
