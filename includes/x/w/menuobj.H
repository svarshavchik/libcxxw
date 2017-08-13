/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_menuobj_h
#define x_w_menuobj_h

#include <x/w/menufwd.H>
#include <x/w/focusable_containerobj.H>
LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see menu

class menuObj : public focusable_containerObj {

public:
	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	menuObj(const ref<implObj> &impl,
		const ref<layoutmanagerObj::implObj> &layout_impl)
		LIBCXX_HIDDEN;

	//! Destructor
	~menuObj() LIBCXX_HIDDEN;

	//! Return the internal focusable object.
	ref<focusableImplObj> get_impl() const override LIBCXX_HIDDEN;

	//! Override get_layout_impl()

	//! Returns the layout manager for the underlying popup.

	ref<layoutmanagerObj::implObj> get_layout_impl() const override
		LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
