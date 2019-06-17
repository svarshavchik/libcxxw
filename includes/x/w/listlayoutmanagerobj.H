/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_listlayoutmanagerobj_h
#define x_w_listlayoutmanagerobj_h

#include <x/w/listlayoutmanagerfwd.H>
#include <x/w/layoutmanagerobj.H>
#include <x/w/copy_cut_paste_menu_itemsfwd.H>
#include <x/w/elementfwd.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see listlayoutmanager

class listlayoutmanagerObj : public layoutmanagerObj,
			     virtual public obj {

public:
	class LIBCXX_HIDDEN implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	listlayoutmanagerObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~listlayoutmanagerObj() LIBCXX_HIDDEN;

	//! Remove a list item
	void remove_item(size_t item_number);

	//! Remove multiple consecutive items.
	void remove_items(size_t item_number,
			 size_t n_items);

	//! Remove a list item
	void remove_item(ONLY IN_THREAD, size_t item_number);

	//! Remove multiple consecutive items.
	virtual void remove_items(ONLY IN_THREAD, size_t item_number,
				  size_t n_items);

	//! Append new list items.

	virtual new_items_ret append_items(const std::vector<list_item_param>
					   &items);

	//! Append new list items.

	virtual new_items_ret append_items(ONLY IN_THREAD,
					   const std::vector<list_item_param>
					   &items);

	//! Insert new list items.
	virtual new_items_ret insert_items(size_t item_number,
					   const std::vector<list_item_param>
					   &items);

	//! Insert new list items.
	virtual new_items_ret insert_items(ONLY IN_THREAD, size_t item_number,
					   const std::vector<list_item_param>
					   &items);

	//! Replace list items.

	virtual new_items_ret replace_items(size_t item_number,
					    const std::vector<list_item_param>
					    &items);

	//! Replace list items.

	virtual new_items_ret replace_items(ONLY IN_THREAD,
					    size_t item_number,
					    const std::vector<list_item_param>
					    &items);

	//! Replace all list items.

	virtual new_items_ret replace_all_items(const
						std::vector<list_item_param>
						&items);

	//! Replace all list items.

	virtual new_items_ret replace_all_items(ONLY IN_THREAD,
						const
						std::vector<list_item_param>
						&items);

	//! Resort items.
	virtual void resort_items(const std::vector<size_t> &indexes);

	//! Resort items.
	virtual void resort_items(ONLY IN_THREAD,
				  const std::vector<size_t> &indexes);

	//! Number of items in the list

	size_t size() const;

	//! Whether list item \#i is selected right now.
	bool selected(size_t i) const;

	//! Return indentation level of the given widget.
	size_t hierindent(size_t i) const;

	//! Return the currently selected item.

	//! For convenience of a single_selection_type() list, returns
	//! the index of the currently selected item.

	std::optional<size_t> selected() const;

	//! Return a list of all selected items.
	std::vector<size_t> all_selected() const;

	//! Select or unselect item \#i
	void selected(size_t i, bool selected_flag);

	//! Select or unselect item \#i
	void selected(ONLY IN_THREAD, size_t i, bool selected_flag,
		      const callback_trigger_t &trigger);

	//! Which widget is currently under cursor
	std::optional<size_t> current_list_item() const;

	//! As if item \#i was clicked on.

	void autoselect(ONLY IN_THREAD,
			size_t i,
			const callback_trigger_t &trigger);
	//! Shortcut

	//! Invoke autoselect() with the default trigger.
	//! \overload
	void autoselect(size_t i);

	//! Unselect all selected items.
	void unselect();

	//! Unselect all selected items.
	void unselect(ONLY IN_THREAD);

	//! Whether the given list item is enabled().

	bool enabled(size_t i) const;

	//! Enable or disable the list item.

	void enabled(size_t i, bool flag);

	//! Enable or disable the list item.

	void enabled(ONLY IN_THREAD, size_t i, bool flag);

	//! Replace/install an item's status callback
	void on_status_update(size_t i,
			      const list_item_status_change_callback &cb);

	//! Replace/install an item's status callback
	void on_status_update(ONLY IN_THREAD,
			      size_t i,
			      const list_item_status_change_callback &cb);

	//! Return an existing menu item's submenu layoutmanager.
	listlayoutmanagerptr get_item_layoutmanager(size_t i);

	//! Append Copy/Cut/Paste menu items.

	copy_cut_paste_menu_items append_copy_cut_paste(const element &parent);

	//! Append Copy/Cut/Paste menu items.

	copy_cut_paste_menu_items append_copy_cut_paste(ONLY IN_THREAD,
							const element &parent);
};

LIBCXXW_NAMESPACE_END

#endif
