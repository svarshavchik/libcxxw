/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_custom_comboboxlayoutmanagerobj_h
#define x_w_custom_comboboxlayoutmanagerobj_h

#include <x/w/custom_comboboxlayoutmanagerfwd.H>
#include <x/w/listlayoutmanagerobj.H>
#include <x/w/layoutmanagerobj.H>

LIBCXXW_NAMESPACE_START

//! Custom combo-box layout manager object.

//! \see custom_comboboxlayoutmanager

class custom_comboboxlayoutmanagerObj : public listlayoutmanagerObj {

public:
	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	custom_comboboxlayoutmanagerObj(const ref<implObj> &impl,
					const ref<listlayoutmanagerObj::implObj>
					&list_layout_impl) LIBCXX_HIDDEN;

	//! Destructor
	~custom_comboboxlayoutmanagerObj() LIBCXX_HIDDEN;

	//! Callback to set the current selection element

	//! Receives a lock on the underlying list and the list item that's
	//! been selected, and the current selection element.

	typedef std::function<void (list_lock &lock,
				    size_t i,
				    const element &)
			      > set_current_selection_callback_t;

	//! Install a callback to set the current selection element.

	//! Use this before creating the combo-box items (using append_item(),
	//! insert_item()) in order to set the callback to update the
	//! combo-box's current selection element when the corresponding
	//! list item gets selectd.

	void set_current_selection_callback(const
					    set_current_selection_callback_t &);

};

LIBCXXW_NAMESPACE_END

#endif
