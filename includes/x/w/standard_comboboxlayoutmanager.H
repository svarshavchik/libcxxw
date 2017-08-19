/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_standard_comboboxlayoutmanager_h
#define x_w_standard_comboboxlayoutmanager_h

#include <x/w/standard_comboboxlayoutmanagerfwd.H>
#include <x/w/standard_comboboxlayoutmanagerobj.H>
#include <x/w/custom_comboboxlayoutmanager.H>
#include <x/w/text_param.H>

LIBCXXW_NAMESPACE_START

//! Information passed to the \ref standard_combobox_selection_changed_t "standard combo-box selection changed callback".

//! This callback gets invoked whenever a standard combo-box currently
//! selectem item changes.

struct LIBCXX_HIDDEN standard_combobox_selection_changed_info_t {

	//! A lock on combo-box's underlying list
	standard_combobox_lock &lock;

	//! Which combo-box item has been selected or unselected
	size_t item_index;

	//! Whether the specified item has been selected or unselected.

	//! When a new combo-box item gets selected the callback may get
	//! invoked twice: once specifying the previous item number,
	//! with a \c selected_flag of \c false; and a second time with the
	//! new \c item_index, and a \c selected_flag of \c true.

	bool selected_flag;

	//! Available mcguffin for blocking all further processing.
	const busy &mcguffin;
};

/*!

Callback that gets invoked when a standard combo-box's selection changes.

*/

typedef std::function<void (const standard_combobox_selection_changed_info_t &)
		      > standard_combobox_selection_changed_t;

/*! Create a standard combobox.

Passed to create_focusable_container() to create a focusable container
with a \ref standard_comboboxlayoutmanager "standard combo-box layout manager".

\code
INSERT_LIBX_NAMESPACE::w::new_standard_comboboxlayoutmanager cclm{
    []
    (const standard_combobox_selection_changed_info_t &info)
    {
    }
};
\endcode

The members/methods of this class are not thread-safe. It is intended that
an execution thread constructs an instance of this class in automatic scope,
then passes the class instance to create_focusable_container().

*/

class LIBCXX_HIDDEN new_standard_comboboxlayoutmanager
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

	//! Callback that gets invoked when the combo-box selection changes.

	standard_combobox_selection_changed_t selection_changed;

 protected:

	//! Internal constructor

	//! \internal
	virtual ref<custom_comboboxlayoutmanagerObj::implObj
		    > create_impl(const create_impl_info &) const;

	//! Return the selection_changed callback.

	//! \internal
	custom_combobox_selection_changed_t get_selection_changed()
		const override;
};

//! Block all other threads from making changes to this combo-box.

//! A subclass of a \ref standard_combobox_lock "standard_combobox_lock"
//! that's constructible from a \ref const_standard_comboboxlayoutmanager "const_standard_comboboxlayoutmanager"
//! with size() and item() methods.

class const_standard_combobox_lock : public list_lock {

protected:
	// For internal use.

	std::vector<text_param> &text_items() const;
public:

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

	void append_item(const text_param &item);

	//! Insert a new item to the combo-box before an existing item.
	void insert_item(size_t i, const text_param &item);

	//! Remove an item from the combo-box.
	void remove_item(size_t i);

	//! Replace an item from the combo-box.
	void replace_item(size_t i, const text_param &item);

	//! Replace all existing items in the combo-box.
	void replace_all_items(const std::vector<text_param> &items);

	//! Combo-box search.

	//! Mostly for internal use.
	bool search(size_t starting_index,
		    const std::u32string &text,
		    size_t &found,
		    bool shortest_match) const LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
