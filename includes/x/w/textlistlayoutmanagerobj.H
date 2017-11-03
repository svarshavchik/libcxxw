/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_textlistlayoutmanagerobj_h
#define x_w_textlistlayoutmanagerobj_h

#include <x/w/textlistlayoutmanagerfwd.H>
#include <x/w/listlayoutmanagerbaseobj.H>
#include <x/w/layoutmanagerobj.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see textlistlayoutmanager

class textlistlayoutmanagerObj : public layoutmanagerObj,
				 public listlayoutmanagerbaseObj {

public:
	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	textlistlayoutmanagerObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~textlistlayoutmanagerObj() LIBCXX_HIDDEN;

	//! Remove a list item
	void remove_item(size_t item_number) override;

	using listlayoutmanagerbaseObj::append_item;

	//! Append new list items.

	void append_item(const std::vector<text_param> &items) override;

	//! Append new list items.

	virtual void append_item(const std::vector<list_item_param> &items);

	using listlayoutmanagerbaseObj::insert_item;

	//! Insert new list items.

	void insert_item(size_t item_number,
			 const std::vector<text_param> &items) override;

	//! Insert new list items.

	virtual void insert_item(size_t item_number,
				 const std::vector<list_item_param> &items);

	using listlayoutmanagerbaseObj::replace_item;

	//! Replace list items.

	void replace_item(size_t item_number,
			  const std::vector<text_param> &items) override;

	//! Replace list items.

	virtual void replace_item(size_t item_number,
				  const std::vector<list_item_param> &items);

	//! Replace all list items.

	void replace_all_items(const std::vector<text_param> &items) override;

	//! Replace all list items.

	virtual void replace_all_items(const std::vector<list_item_param>
				       &items);

	//! Number of items in the list

	size_t size() const override;

	//! Whether list item \#i is selected right now.
	bool selected(size_t i) const override;

	//! Return the currently selected item.

	//! For convenience of a single_selection_type() list, returns
	//! the index of the currently selected item.

	std::optional<size_t> selected() const override;

	//! Return a list of all selected items.
	std::vector<size_t> all_selected() const override;

	//! Select or unselect item \#i
	void selected(size_t i, bool selected_flag,
		      const callback_trigger_t &trigger) override;

	using listlayoutmanagerbaseObj::selected;

	//! As if item \#i was clicked on.

	void autoselect(size_t i,
			const callback_trigger_t &trigger) override;
	using listlayoutmanagerbaseObj::autoselect;

	//! Unselect all selected items.
	void unselect() override;

	//! Whether the given list item is enabled().

	bool enabled(size_t i) const override;

	//! Enable or disable the list item.

	void enabled(size_t i, bool flag) override;

	//! Return an existing menu item's submenu layoutmanager.
	textlistlayoutmanagerptr get_item_layoutmanager(size_t i);
};

LIBCXXW_NAMESPACE_END

#endif
