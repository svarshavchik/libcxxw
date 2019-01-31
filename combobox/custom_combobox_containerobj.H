/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef custom_combobox_containerobj_h
#define custom_combobox_containerobj_h

#include "combobox/custom_combobox_containerfwd.H"
#include "image_button_internalfwd.H"

#include "x/w/focusable_containerobj.H"
#include "x/w/custom_comboboxlayoutmanagerobj.H"
#include "x/w/impl/popup/popupfwd.H"

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see custom_combobox_container

class LIBCXX_HIDDEN custom_combobox_containerObj
	: public focusable_containerObj {

public:
	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! My layout manager.
	const ref<custom_comboboxlayoutmanagerObj::implObj> layout_manager_impl;

	//! The current selection focusable

	const focusable current_selection_focusable;

	//! The button to open/hide the popup.

	const image_button_internal combobox_button;

	//! The combo-box popup window.

	const popup popup_window;

	//! Constructor
	custom_combobox_containerObj(const ref<implObj> &impl,
				     const ref<custom_comboboxlayoutmanagerObj
				     ::implObj> &layout_manager_impl,
				     const focusable
				     &current_selection_focusable,
				     const image_button_internal
				     &combobox_button,
				     const popup &popup_window) LIBCXX_HIDDEN;

	//! Destructor
	~custom_combobox_containerObj() LIBCXX_HIDDEN;

	//! Inherited from focusableObj.
	focusable_impl get_impl() const override LIBCXX_HIDDEN;
	//! Inherited from focusableObj.
	void do_get_impl(const function<internal_focusable_cb> &)
		const override LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
