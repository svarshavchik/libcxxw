/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_menuobj_h
#define x_w_menuobj_h

#include <x/w/menufwd.H>
#include <x/w/listlayoutmanagerfwd.H>
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
		const layout_impl &container_layout_impl)
		LIBCXX_HIDDEN;

	//! Destructor
	~menuObj() LIBCXX_HIDDEN;

	//! Return the internal focusable object.
	focusable_impl get_impl() const override LIBCXX_HIDDEN;

	//! Override get_layout_impl()

	//! Returns the layout manager for the underlying popup.

	layout_impl get_layout_impl() const override
		LIBCXX_HIDDEN;

	//! Return this menu's layout manager.
	listlayoutmanager get_layoutmanager();

	//! Return this menu's layout manager.
	const_listlayoutmanager get_layoutmanager() const;
};

LIBCXXW_NAMESPACE_END

#endif
