/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_popup_popupobj_h
#define x_w_impl_popup_popupobj_h

#include "x/w/impl/popup/popupfwd.H"
#include "x/w/generic_windowobj.H"

LIBCXXW_NAMESPACE_START

//! A popup "public" object.

class popupObj : public generic_windowObj {

public:

	class implObj;
	class handlerObj;

	//! Implementation object.

	const ref<implObj> impl;

	//! Constructor
	popupObj(const ref<implObj> &impl,
		 const layout_impl &layout) LIBCXX_HIDDEN;

	//! Destructor
	~popupObj() LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
