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
	//! Alias
	typedef custom_comboboxlayoutmanagerObj superclass_t;

	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	standard_comboboxlayoutmanagerObj(const ref<implObj> &impl,
					  const ref<textlistlayoutmanagerObj
					  ::implObj>
					  &list_layout_impl) LIBCXX_HIDDEN;

	//! Destructor
	~standard_comboboxlayoutmanagerObj() LIBCXX_HIDDEN;

	using custom_comboboxlayoutmanagerObj::append_item;

	//! Append a new item to the combo-box.

	void append_item(const std::vector<list_item_param> &items) override;

	using custom_comboboxlayoutmanagerObj::insert_item;

	//! Insert a new item to the combo-box before an existing item.

	void insert_item(size_t i, const std::vector<list_item_param> &items)
		override;

	//! Remove an item from the combo-box.

	void remove_item(size_t i) override;

	using custom_comboboxlayoutmanagerObj::replace_item;

	//! Replace an item.

	void replace_item(size_t item_number,
			  const std::vector<list_item_param> &items) override;

	using custom_comboboxlayoutmanagerObj::replace_all_items;

	//! Override replace_all().

	void replace_all_items(const std::vector<list_item_param> &items) override;

	//! Return combo-box item.
	text_param item(size_t i) const;
};

LIBCXXW_NAMESPACE_END

#endif
