/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_standard_comboboxlayoutmanager_h
#define x_w_standard_comboboxlayoutmanager_h

#include <x/w/standard_comboboxlayoutmanagerfwd.H>
#include <x/w/standard_comboboxlayoutmanagerobj.H>
#include <x/w/custom_comboboxlayoutmanager.H>
#include <x/w/text_param.H>
#include <x/functionalrefptr.H>

LIBCXXW_NAMESPACE_START

//! Information passed to the \ref standard_combobox_selection_changed_t "standard combo-box selection changed callback".

//! This callback gets invoked whenever a standard combo-box currently
//! selectem item changes.

struct standard_combobox_selection_changed_info_t {

	//! The layout manager
	const standard_comboboxlayoutmanager &lm;

	//! A lock on combo-box's underlying list
	standard_combobox_lock &lock;

	//! Selected item status metadata

	//! Information from the underlying item selection.
	const list_item_status_info_t &list_item_status_info;

	//! The busy mcguffin factory, for the callback to use if it wants to.
	const busy &mcguffin;
};

/*! Create a standard combobox.

Passed to create_focusable_container() to create a focusable container
with a \ref standard_comboboxlayoutmanager "standard combo-box layout manager":

\code
INSERT_LIBX_NAMESPACE::w::new_standard_comboboxlayoutmanager sclm{
    []
    (const standard_combobox_selection_changed_info_t &info)
    {
    }
};

auto new_container=factory->create_focusable_container(
    []
    (const INSERT_LIBX_NAMESPACE::w::focusable_container &new_container)
    {
    },
    sclm);

\endcode

The members/methods of this class are not thread-safe. It is intended that
an execution thread constructs an instance of this class in automatic scope,
then passes the class instance to create_focusable_container().

\par Synchronized combo-boxes

The width of each individual combo-box gets sized to be just wide enough to
show the widest combo-box item. It's possible to create several combo-boxes
with the same width, sizing all combo-boxes's width based on the wide item
in all the combo-boxes, combined together.

This is simply done by using the same \c new_standard_comboboxlayoutmanager
object. See
\ref new_listlayoutmanager "INSERT_LIBX_NAMESPACE::w::new_listlayoutmanager"
for more information.

*/

class new_standard_comboboxlayoutmanager
	: public new_custom_comboboxlayoutmanager {

 public:

	//! Constructor
	new_standard_comboboxlayoutmanager() LIBCXX_PUBLIC;

	//! Constructor
	new_standard_comboboxlayoutmanager
		(const standard_combobox_selection_changed_t
		 &selection_changed) LIBCXX_PUBLIC;

	//! Destructor
	~new_standard_comboboxlayoutmanager() LIBCXX_PUBLIC;
private:
	//! Implement selection_factory()
	focusable selection_factory(const factory &) const override
		LIBCXX_HIDDEN;
public:
	//! Callback that gets invoked when the combo-box selection changes.

	standard_combobox_selection_changed_t selection_changed;

 protected:

	//! Internal constructor

	//! \internal
	ref<custom_comboboxlayoutmanagerObj::implObj>
		create_impl(const create_impl_info &) const override;

	//! Return the selection_changed callback.

	//! \internal
	custom_combobox_selection_changed_t get_selection_changed()
		const override;
};

//! Block all other threads from making changes to this combo-box.

//! A subclass of a \ref standard_combobox_lock "standard_combobox_lock"
//! that's constructible from a \ref const_standard_comboboxlayoutmanager "const_standard_comboboxlayoutmanager"
//! with size() and item() methods.

class const_standard_combobox_lock : public const_list_lock {

public:
	// For internal use.

	std::vector<text_param> &text_items() const LIBCXX_HIDDEN;


	friend class standard_comboboxlayoutmanagerObj;
	friend class standard_comboboxlayoutmanagerObj::implObj;

	const const_standard_comboboxlayoutmanager locked_layoutmanager;

	//! Constructor
	const_standard_combobox_lock(const const_standard_comboboxlayoutmanager &);

	//! Destructor
	~const_standard_combobox_lock();

	//! How many items in the combo-box.
	size_t size() const;

	//! Return combo-box item.
	text_param item(size_t i) const;
};

//! Block all other threads from making changes to this combo-box.

class standard_combobox_lock : public const_standard_combobox_lock {

public:
	friend class standard_comboboxlayoutmanagerObj;
	friend class standard_comboboxlayoutmanagerObj::implObj;

	const standard_comboboxlayoutmanager locked_layoutmanager;

	//! Constructor
	standard_combobox_lock(const standard_comboboxlayoutmanager &);

	//! Destructor
	~standard_combobox_lock();

	//! Append a new item to the combo-box.
	new_items_ret append_items(const list_item_param &item);

	//! Append new items to the combo-box.
	new_items_ret append_items(const std::vector<list_item_param> &items);

	//! Append new items to the combo-box.
	new_items_ret append_items(ONLY IN_THREAD,
				   const std::vector<list_item_param> &items);

	//! Insert a new item to the combo-box before an existing item.
	new_items_ret insert_items(size_t i, const list_item_param &item);

	//! Insert a new item to the combo-box before an existing item.
	new_items_ret insert_items(size_t i,
				   const std::vector<list_item_param> &items);

	//! Insert a new item to the combo-box before an existing item.
	new_items_ret insert_items(ONLY IN_THREAD,
				   size_t i,
				   const std::vector<list_item_param> &items);

	//! Remove an item from the combo-box.
	void remove_item(size_t i);

	//! Remove an item from the combo-box.
	void remove_item(ONLY IN_THREAD, size_t i);

	//! Remove items from the combo-box.
	void remove_items(size_t i, size_t n);

	//! Remove items from the combo-box.
	void remove_items(ONLY IN_THREAD, size_t i, size_t n);

	//! Replace an item in the combo-box.
	new_items_ret replace_items(size_t i, const list_item_param &item);

	//! Replace items in the combo-box.
	new_items_ret replace_items(size_t i,
				    const std::vector<list_item_param> &items);

	//! Replace items in the combo-box.
	new_items_ret replace_items(ONLY IN_THREAD,
				    size_t i,
				    const std::vector<list_item_param> &items);

	//! Replace all existing items in the combo-box.
	new_items_ret replace_all_items(const std::vector<list_item_param>
					&items);

	//! Replace all existing items in the combo-box.
	new_items_ret replace_all_items(ONLY IN_THREAD,
					const std::vector<list_item_param>
					&items);

	//! Re-sort the existing items in the combo-box.
	void resort_items(const std::vector<size_t> &indexes);

	//! Re-sort the existing items in the combo-box.
	void resort_items(ONLY IN_THREAD, const std::vector<size_t> &indexes);

	//! Combo-box search.

	//! Mostly for internal use.
	bool search(size_t starting_index,
		    const std::u32string &text,
		    size_t &found,
		    bool shortest_match) const LIBCXX_HIDDEN;

};

LIBCXXW_NAMESPACE_END

#endif
