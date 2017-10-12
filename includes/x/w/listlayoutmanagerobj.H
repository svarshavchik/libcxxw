/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_listlayoutmanagerobj_h
#define x_w_listlayoutmanagerobj_h

#include <x/w/listlayoutmanagerfwd.H>
#include <x/w/listlayoutmanagerbaseobj.H>
#include <x/w/grid_map_infofwd.H>
#include <x/w/layoutmanagerobj.H>
#include <x/w/factoryfwd.H>
#include <x/w/factoryobj.H>
#include <x/w/text_paramfwd.H>

#include <x/mpobj.H>

#include <vector>

LIBCXXW_NAMESPACE_START

//! The list layout manager

//! \see listlayoutmanager

class listlayoutmanagerObj : public layoutmanagerObj,
			     public listlayoutmanagerbaseObj {

public:
	friend class gridfactoryObj;

	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	listlayoutmanagerObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~listlayoutmanagerObj() LIBCXX_HIDDEN;

	using listlayoutmanagerbaseObj::append_item;

	//! Append a new list item.
	virtual factory append_item();

	//! Append a new list item

	//! \overload
	void append_item(const std::vector<text_param> &items) override;

	using listlayoutmanagerbaseObj::insert_item;

	//! Insert a new list item.
	virtual factory insert_item(size_t item_number);

	//! Insert a new list item

	//! \overload
	void insert_item(size_t item_number,
			 const std::vector<text_param> &items) override;

	//! Replace a list item.
	virtual factory replace_item(size_t item_number);


	using listlayoutmanagerbaseObj::replace_item;

	//! Replace a list item

	//! \overload

	void replace_item(size_t item_number,
			  const std::vector<text_param> &items) override;

	//! Remove a list item
	void remove_item(size_t item_number) override;

	using listlayoutmanagerbaseObj::replace_all_items;

	//! Replace all list items

	virtual factory replace_all_items();

	//! Replace all list items

	//! \overload
	void replace_all_items(const std::vector<text_param> &items) override;

	///////////////////////////////////////////////////////////////

	//! Number of items in the list

	size_t size() const override;

	//! Number of items in the list
	size_t size(grid_map_t::lock &lock) const;

	using listlayoutmanagerbaseObj::selected;

	//! Whether list item \#i is selected right now.
	bool selected(size_t i) const override;

	//! Whether list item \#i is selected right now.
	bool selected(grid_map_t::lock &lock, size_t i) const;

	//! Return the currently selected item.

	//! For convenience of a single_selection_type() list, returns
	//! the index of the currently selected item.

	std::optional<size_t> selected() const override;

	//! Return a list of all selected items.
	std::vector<size_t> all_selected() const override;

	//! Select or unselect item \#i
	void selected(size_t i, bool selected_flag,
		      const callback_trigger_t &trigger) override;

	//! Select or unselect item \#i
	void selected(grid_map_t::lock &lock, size_t i, bool selected_flag,
		      const callback_trigger_t &trigger);

	//! Select or unselect item \#i
	void selected(grid_map_t::lock &lock, size_t i, bool selected_flag);

	using listlayoutmanagerbaseObj::autoselect;

	//! As if item \#i was clicked on.
	void autoselect(size_t i,
			const callback_trigger_t &trigger) override;

	//! As if item \#i was clicked on.
	void autoselect(grid_map_t::lock &lock, size_t i,
			const callback_trigger_t &trigger);

	//! As if item \#i was clicked on.

	//! \overload
	void autoselect(grid_map_t::lock &lock, size_t i);

	//! Unselect all selected items.
	void unselect() override;

	//! Return the item element.
	element item(size_t item_number, size_t column=0);

	//! Whether the given list item is enabled().

	bool enabled(size_t i) const override;

	//! Whether the given list item is enabled().

	bool enabled(const element &e) const;

	//! Enable or disable the list item.

	void enabled(size_t i, bool flag) override;

	//! Enable or disable the list item.

	void enabled(const element &e, bool flag);

	//! Add a text item, (or a separator row).

	//! \internal
	void append_text_or_separator(const factory &f,
				      const text_param &t) LIBCXX_HIDDEN;

	//! Add a text item, (or a separator row).

	//! \internal
	void insert_text_or_separator(size_t item_number,
				      const factory &f,
				      const text_param &t) LIBCXX_HIDDEN;

	//! Add a text item, (or a separator row).

	//! \internal
	void replace_text_or_separator(size_t item_number,
				       const factory &f,
				       const text_param &t) LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
