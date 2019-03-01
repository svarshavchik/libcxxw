/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_override_redirect_h
#define x_w_override_redirect_h

#include "override_redirectfwd.H"
#include "override_redirectobj.H"
#include "generic_window_handler.H"
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

/*! Internally-controlled top level display element with Override-Redirect

The override_redirect_elementObj template multiply-inherits from
override_redirectObj and a generic_windowObj::handlerObj representing a
top level display element that has override redirect set.

The template mixin implements additional handling of override-redirected
window popups.

*/

template<typename element_impl>
class override_redirect_elementObj : public element_impl,
				     public override_redirectObj {

public:

	//! Inherited constructor

	//! The constructor must be invoked with the override redirect flag
	//! set.

	using element_impl::element_impl;

	//! Destructor

	~override_redirect_elementObj()=default;

	//! Implement get_override_redirected_window_handler().

	generic_windowObj::handlerObj &get_override_redirected_window_handler()
		override
	{
		return *this;
	}
};

LIBCXXW_NAMESPACE_END

#endif
