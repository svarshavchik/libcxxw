/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_standard_comboboxlayoutmanagerobj_h
#define x_w_standard_comboboxlayoutmanagerobj_h

#include <x/w/standard_comboboxlayoutmanagerfwd.H>
#include <x/w/custom_comboboxlayoutmanagerobj.H>
#include <x/w/text_paramfwd.H>

LIBCXXW_NAMESPACE_START

//! Standard combo-box layout manager object.

//! \see standard_comboboxlayoutmanager

class standard_comboboxlayoutmanagerObj
	: public custom_comboboxlayoutmanagerObj {

public:
	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	standard_comboboxlayoutmanagerObj(const ref<implObj> &impl,
					  const ref<listlayoutmanagerObj
					  ::implObj>
					  &list_layout_impl) LIBCXX_HIDDEN;

	//! Destructor
	~standard_comboboxlayoutmanagerObj() LIBCXX_HIDDEN;

	//! Append a new item to the combo-box.

	void append_item(const text_param &item);

	//! Insert a new item to the combo-box before an existing item.

	void insert_item(size_t i, const text_param &item);

	//! Remove an item from the combo-box.

	void remove_item(size_t i);

	//! How many items in the combo-box.

	size_t size() const;

	//! Return combo-box item.
	text_param item(size_t i) const;

	//! Replace all existing items in the combo-box.
	void replace_all(const std::vector<text_param> &items);
};

LIBCXXW_NAMESPACE_END

#endif
