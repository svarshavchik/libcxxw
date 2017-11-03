/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_listlayoutmanager_h
#define x_w_listlayoutmanager_h

#include <x/w/new_focusable_layoutmanagerfwd.H>
#include <x/w/alignment.H>
#include <x/w/textlistlayoutmanagerfwd.H>
#include <x/w/listlayoutmanagerbase.H>
#include <x/w/layoutmanager.H>
#include <x/w/scrollbar.H>
#include <x/w/grid_map_infofwd.H>
#include <x/w/text_param.H>
#include <x/ref.H>
#include <x/refptr_traits.H>
#include <x/w/busyfwd.H>
#include <string>
#include <functional>
#include <tuple>
#include <unordered_map>

LIBCXXW_NAMESPACE_START

//! selection_type callback.

//! \see new_listlayoutmanager

typedef std::function<void (const listlayoutmanagerbase &, size_t,
			    const callback_trigger_t &,
			    const busy &)
		      > list_selection_type_cb_t;

//! selection_changed callback.

//! \see new_listlayoutmanager

typedef std::function<void (const listlayoutmanagerbase &,
			    size_t, bool,
			    const callback_trigger_t &,
			    const busy &)
		      > list_selection_changed_cb_t;

//! Create a new container with a \ref listlayoutmanager "list layout manager".

//! This parameter gets passed to a \ref factory "factory"'s
//! \c create_focusable_container()
//! to create a simple selection list.
//!
//! The constructor takes one optional parameter, which can be:
//!
//! - \c INSERT_LIBX_NAMESPACE::w::highlighted_list - selected items in
//! the list are displayed by a highlighted background color. This is the
//! default value.
//!
//! The remaining fields get initialized to their default values and may be
//! modified prior before using \c create_focusable_container().

class LIBCXX_HIDDEN new_listlayoutmanager : public new_focusable_layoutmanager {

 public:

	//! Style of the new list.
	const layout_style_t &layout_style;

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

	list_selection_type_cb_t selection_type;

	//! Callback when list selection changes.

	//! Invoked each time a list item is selected or unselected.
	//!
	//! This callback cannot strongly capture the list display element
	//! or any of its parent display elements.

	list_selection_changed_cb_t selection_changed;

	//! Number of rows in the list.

	//! The default is four rows. The actual size of the list element
	//! is computed based on the number of rows and the default list
	//! font.
	size_t rows;

	//! Number of columns in the list

	//! This is not the number of character, but the number of
	//! elements in each list item. The default is 1.
	size_t columns;

	//! Relative width of columns.

	//! If the selection list is filled horizontally, inside its parent
	//! grid layout manager, the additional space goes to these columns.
	std::unordered_map<size_t, int> requested_col_widths;

	//! Alignment of all columns.
	std::unordered_map<size_t, halign> col_alignments;

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
	new_listlayoutmanager(const layout_style_t &v=text_list)
		LIBCXX_PUBLIC;

	//! Destructor
	~new_listlayoutmanager() LIBCXX_PUBLIC;

	//! \internal
	//! Create the focusable container.

	focusable_container create(const ref<containerObj::implObj> &)
		const override;
};

//! Lock the list layout manager.

//! Blocks modifications to the contents of the list.

class list_lock : public listimpl_info_lock_t {

public:

	const const_textlistlayoutmanager layout_manager;

	//! Disambiguating constructor
	template<typename ref_or_ptr, typename=
		 typename refptr_traits<ref_or_ptr>::obj_t>
	inline list_lock(const ref_or_ptr &manager)
		: list_lock(*manager)
	{
	}

	//! Constructor
	list_lock(const textlistlayoutmanagerObj &manager);

	//! Destructor
	~list_lock();
};

LIBCXXW_NAMESPACE_END

#endif
