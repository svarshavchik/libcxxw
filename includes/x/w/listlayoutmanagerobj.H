/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_listlayoutmanagerobj_h
#define x_w_listlayoutmanagerobj_h

#include <x/w/listlayoutmanagerfwd.H>
#include <x/w/grid_map_infofwd.H>
#include <x/w/layoutmanagerobj.H>
#include <x/w/factoryfwd.H>
#include <x/w/text_paramfwd.H>

#include <vector>
#include <initializer_list>

LIBCXXW_NAMESPACE_START

//! The grid layout manager

//! \see listlayoutmanager

class listlayoutmanagerObj : public layoutmanagerObj {

public:
	friend class gridfactoryObj;

	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	listlayoutmanagerObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~listlayoutmanagerObj() LIBCXX_HIDDEN;

	//! Append a new list item.
	factory append_item();

	//! Append a new list item

	//! \overload
	template<typename ...Args>
	void append_item(const text_param &text, Args && ...args)
	{
		std::vector<text_param>
			items{text, std::forward<Args>(args)...};
		append_item(items);
	}

	//! Append a new list item

	//! \overload
	void append_item(const std::vector<text_param> &items);

	//! Insert a new list item.
	factory insert_item(size_t item_number);

	//! Insert a new list item

	//! \overload
	template<typename ...Args>
	void insert_item(size_t item_number,
			 const text_param &text, Args && ...args)
	{
		std::vector<text_param>
			items{text, std::forward<Args>(args)...};
		insert_item(item_number, items);
	}

	//! Insert a new list item

	//! \overload
	void insert_item(size_t item_number,
			 const std::vector<text_param> &items);

	//! Replace a list item.
	factory replace_item(size_t item_number);

	//! Replace a list item

	//! \overload
	template<typename ...Args>
	void replace_item(size_t item_number,
			  const text_param &text, Args && ...args)
	{
		std::vector<text_param>
			items{text, std::forward<Args>(args)...};
		replace_item(item_number, items);
	}

	//! Replace a list item

	//! \overload

	void replace_item(size_t item_number,
			 const std::vector<text_param> &items);

	//! Remove a list item
	void remove_item(size_t item_number);

	//! Replace all list items
	factory replace_all();

	//! Replace all list items

	//! \overload
	void replace_all(const std::vector<text_param> &items);

	///////////////////////////////////////////////////////////////

	//! Number of items in the list

	size_t size() const;

	//! Number of items in the list
	size_t size(grid_map_t::lock &lock) const;

	//! Whether list item \#i is selected right now.
	bool selected(size_t i) const;

	//! Whether list item \#i is selected right now.
	bool selected(grid_map_t::lock &lock, size_t i) const;

	//! Select or unselect item \#i
	void selected(size_t i, bool selected_flag);

	//! Select or unselect item \#i
	void selected(grid_map_t::lock &lock, size_t i, bool selected_flag);

	//! As if item \#i was clicked on.
	void autoselect(size_t i);

	//! As if item \#i was clicked on.
	void autoselect(grid_map_t::lock &lock, size_t i);
};

LIBCXXW_NAMESPACE_END

#endif
