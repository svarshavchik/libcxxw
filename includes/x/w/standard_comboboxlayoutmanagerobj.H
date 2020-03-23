/*
** Copyright 2017-2020 Double Precision, Inc.
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

	class LIBCXX_HIDDEN implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	standard_comboboxlayoutmanagerObj(const ref<implObj> &impl,
					  const ref<listlayoutmanagerObj
					  ::implObj>
					  &list_layout_impl) LIBCXX_HIDDEN;

	//! Destructor
	~standard_comboboxlayoutmanagerObj() LIBCXX_HIDDEN;

	//! Override current_selection()

	//! The standard combo-box's current selection is internal, and
	//! inaccessible. This throws an exception.
	element current_selection() override;

	//! Override current_selection()

	//! The standard combo-box's current selection is internal, and
	//! inaccessible. This throws an exception.
	const_element current_selection() const override;

	//! Append a new item to the combo-box.

	new_items_ret append_items(const std::vector<list_item_param> &items)
		override;

	//! Append a new item to the combo-box.

	new_items_ret append_items(ONLY IN_THREAD,
				   const std::vector<list_item_param> &items)
		override;

	//! Insert a new item to the combo-box before an existing item.

	new_items_ret insert_items(size_t i,
				   const std::vector<list_item_param> &items)
		override;

	//! Insert a new item to the combo-box before an existing item.

	new_items_ret insert_items(ONLY IN_THREAD,
				   size_t i,
				   const std::vector<list_item_param> &items)
		override;

	//! Remove an item from the combo-box.
	void remove_items(ONLY IN_THREAD, size_t i, size_t n_items)
		override;

	void remove_items(size_t item_number,
			  size_t n_items) override;

	//! Replace an item.

	new_items_ret replace_items(size_t item_number,
				    const std::vector<list_item_param> &items)
		override;

	//! Replace an item.

	new_items_ret replace_items(ONLY IN_THREAD,
				    size_t item_number,
				    const std::vector<list_item_param> &items)
		override;

	//! Override replace_all().

	new_items_ret replace_all_items(const std::vector<list_item_param>
					&items)
		override;

	//! Override replace_all().

	new_items_ret replace_all_items(ONLY IN_THREAD,
					const std::vector<list_item_param>
					&items)
		override;

	using superclass_t::resort_items;

	//! Override resort_items().
	void resort_items(ONLY IN_THREAD,
			  const std::vector<size_t> &indexes) override;

	//! Return combo-box item.
	text_param item(size_t i) const;

	//! Install a new selection changed callback.
	void on_selection_changed(const standard_combobox_selection_changed_t &)
		;

	//! Install a new selection changed callback.
	void on_selection_changed(ONLY IN_THREAD,
				  const standard_combobox_selection_changed_t &)
		;

	//! Implement generate()

	//! \see uigenerators

	void generate(const std::string_view &name,
		      const const_uigenerators &generators,
		      uielements &elements) override;

	using custom_comboboxlayoutmanagerObj::generate;
};

LIBCXXW_NAMESPACE_END

#endif
