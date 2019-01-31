/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_menuobj_h
#define x_w_menuobj_h

#include <x/w/menufwd.H>
#include <x/w/listlayoutmanagerfwd.H>
#include <x/w/focusable_containerobj.H>
#include <x/functionalrefptrfwd.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see menu

class menuObj : public focusable_containerObj {

public:
	class LIBCXX_HIDDEN implObj;

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

	//! Install a state update callback for the menu popup.
	void on_popup_state_update(const functionref<element_state_callback_t>
				   &callback);

	//! Install a state update callback for the menu popup.
	void on_popup_state_update(ONLY IN_THREAD,
				   const functionref<element_state_callback_t>
				   &callback);
};

LIBCXXW_NAMESPACE_END

#endif
