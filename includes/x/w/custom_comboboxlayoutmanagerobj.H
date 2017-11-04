/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_custom_comboboxlayoutmanagerobj_h
#define x_w_custom_comboboxlayoutmanagerobj_h

#include <x/w/custom_comboboxlayoutmanagerfwd.H>
#include <x/w/listlayoutmanagerobj.H>
#include <x/w/layoutmanagerobj.H>
#include <x/w/elementfwd.H>

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
					const ref<listlayoutmanagerObj
					::implObj>
					&list_layout_impl) LIBCXX_HIDDEN;

	//! Destructor
	~custom_comboboxlayoutmanagerObj() LIBCXX_HIDDEN;

	//! Return the display element that represents the current selection.
	element current_selection();

	//! Return the display element that represents the current selection.
	const_element current_selection() const;
};

LIBCXXW_NAMESPACE_END

#endif
