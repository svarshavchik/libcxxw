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
#include <x/w/text_param.H>
#include <x/ref.H>
#include <x/w/busyfwd.H>
#include <string>
#include <functional>

LIBCXXW_NAMESPACE_START

//! Create a new container with a \ref listlayoutmanager "list layout manager".

//! This parameter gets passed to a \ref factory "factory"'s
//! \c create_focusable_container()
//! to create a simple selection list.
//!
//! The constructor takes one parameter, which can be:
//!
//! - \c INSERT_LIBX_NAMESPACE::w::highlighted_list - selecteded items in
//! the list are displayed by a highlighted background color.
//!
//! The remaining fields get initialized to their default values and may be
//! modified prior before using \c create_focusable_container().

class LIBCXX_HIDDEN new_listlayoutmanager : public new_focusable_layoutmanager {

 public:

	//! Style of the new list.
	const listlayoutstyle &layout_style;

	//! Single or multiple selection

	//! This may be set to:
	//!
	//! - \c INSERT_LIBX_NAMESPACE::w::single_selection_type (default):
	//! one item may be selected in the list at a time. Selecting a new
	//! item automatically deselects the previous one.
	//!
	//! - \c INSERT_LIBX_NAMESPACE::w::multiple_selection_type:
	//! each item in the list is selected or unselected individually and
	//! more than one item can be selected in the list, at the same time.

	std::function<void (list_lock &, const listlayoutmanager &, size_t)
		      > selection_type;

	//! Callback when list selection changes.

	//! Invoked each time a list item is selected or unselected.
	//!
	//! This callback cannot strongly capture the list display element
	//! or any of its parent display elements.

	std::function<void (const listlayoutmanager &,
			    size_t, bool, const busy &)
		      > selection_changed;

	//! Number of rows in the list.

	//! The default is four rows. The actual size of the list element
	//! is computed based on the number of rows and the default list
	//! font.
	size_t rows;

	//! Number of columns in the list

	//! This is not the number of character, but the number of
	//! elements in each list item. The default is 1.
	size_t columns;

	//! Padding above and below each list item
	std::string v_padding;

	//! Left margin
	std::string left_padding;

	//! Padding between columns of multicolumn lists
	std::string inner_padding;

	//! Right margin padding
	std::string right_padding;

	//! Visibility of the vertical scrollbar
	scrollbar_visibility vertical_scrollbar;

	//! Background color
	std::string background_color;

	//! Selected color

	//! Used only by a \c INSERT_LIBX_NAMESPACE::w::highlighted_list.
	std::string selected_color;

	//! Highlighted color
	std::string highlighted_color;

	//! Currently selected color
	std::string current_color;

	//! Constructor
	new_listlayoutmanager(const listlayoutstyle &) LIBCXX_PUBLIC;

	//! Destructor
	~new_listlayoutmanager() LIBCXX_PUBLIC;

	//! \internal
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
