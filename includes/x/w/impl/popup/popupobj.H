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
		 const layout_impl &layout,
		 const layout_impl &content_layout) LIBCXX_HIDDEN;

private:

	//! Layout manager for the contents of the popup

	const layout_impl content_layout;

public:
	//! Override get_layout_impl()

	//! Returns the layout manager for the apparent contents of the popup.
	//!
	//! The popup container itself uses the border layout manager to draw
	//! borders around the contents of the popup, a separate container
	//! (for a small scrollbar-less popup, or the grid layout manager for
	//! a potentially-scrollable popup).
	//!
	//! Override get_layout_impl() to return that container's layout
	//! manager, so that its contents appear to be the contents of this
	//! popup.
	layout_impl get_layout_impl() const override;

	//! Destructor
	~popupObj() LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
