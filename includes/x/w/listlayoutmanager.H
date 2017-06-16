/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_listlayoutmanager_h
#define x_w_listlayoutmanager_h

#include <x/w/new_focusable_layoutmanagerfwd.H>
#include <x/w/listlayoutmanagerfwd.H>
#include <x/w/listlayoutmanagerobj.H>
#include <x/w/layoutmanager.H>
#include <x/w/scrollbar.H>
#include <x/w/grid_map_infofwd.H>
#include <x/ref.H>
#include <string>

LIBCXXW_NAMESPACE_START

//! A new list layout manager.

//! Passed to \ref factory "factory's" create_focusable_container() to specify
//! the visual appearance of a list.

class LIBCXX_HIDDEN new_listlayoutmanager : public new_focusable_layoutmanager {

 public:

	//! Style of the new list.
	const listlayoutstyle &layout_style;

	//! Style of the new list

	//! Padding above and below each list item
	std::string v_padding;

	//! Left margin
	std::string left_padding;

	//! Padding between columns of multicolumn lists
	std::string inner_padding;

	//! Right margin padding
	std::string right_padding;

	//! Number of rows in the list.
	size_t rows;

	//! Number of columns
	size_t columns;

	//! Visibility of the vertical scrollbar
	scrollbar_visibility vertical_scrollbar;

	//! Constructor
	new_listlayoutmanager(const listlayoutstyle &) LIBCXX_PUBLIC;

	//! Destructor
	~new_listlayoutmanager() LIBCXX_PUBLIC;

	//! Create the focusable container.

	focusable_container create(const ref<containerObj::implObj> &)
		const override;
};

//! Lock the list layout manager.

//! Blocks modifications to the contents of the list.

class list_lock : public grid_map_t::lock {

public:

	//! Constructor

	list_lock(const const_listlayoutmanager &manager);

	//! Constructor
	list_lock(grid_map_t &map);

	//! Destructor
	~list_lock();
};

LIBCXXW_NAMESPACE_END

#endif
