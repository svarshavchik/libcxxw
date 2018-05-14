/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_listlayoutmanager_h
#define x_w_listlayoutmanager_h

#include <x/w/listlayoutmanagerfwd.H>
#include <x/w/listlayoutmanagerobj.H>
#include <x/w/new_focusable_layoutmanagerfwd.H>
#include <x/w/alignment.H>
#include <x/w/layoutmanager.H>
#include <x/w/scrollbar.H>
#include <x/w/grid_map_infofwd.H>
#include <x/w/text_param.H>
#include <x/w/image_param.H>
#include <x/w/shortcut.H>
#include <x/w/rgb.H>
#include <x/w/border_arg.H>
#include <x/w/dim_axis_arg.H>
#include <x/w/font_arg.H>
#include <x/w/synchronized_axisfwd.H>
#include <x/ref.H>
#include <x/refptr_traits.H>
#include <x/functionalrefptr.H>
#include <string>
#include <variant>
#include <unordered_map>

LIBCXXW_NAMESPACE_START

//! selection_changed callback.

//! \see new_listlayoutmanager

typedef functionptr<void (THREAD_CALLBACK, const list_item_status_info_t &)
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
//! - \c INSERT_LIBX_NAMESPACE::w::bulleted_list - a bullet appears next to
//! each selected item in the list.
//!
//! The remaining fields get initialized to their default values and may be
//! modified prior before using \c create_focusable_container().
//!
//! \par Synchronized lists
//!
//! It is possible to use the same \c new_listlayoutmanager to create more
//! than one selection list. Each individual selection list's width gets
//! automatically sized to be big enough to show the widest list item;
//! but using the same \c new_listlayoutmanager makes all individual selection
//! lists have the same width, the width of the widest selection list.
//! This results in a tidy visual appearance when these selection lists appear
//! in the same column.
//!
//! This is due to the default-constructed \c synchronized_columns member.
//! Each selection list constructed with the same
//! \ref synchronized_axis "synchronized axis object" and the same number
//! of columns gets sized together with all selection lists that link to the
//! same synchronized axis object. It is possible to constructed multiple
//! selection lists using different \c new_listlayoutmanager objects, provided
//! that the synchronized axis object gets carefully copied to each one.

class LIBCXX_HIDDEN new_listlayoutmanager : public new_focusable_layoutmanager {

 public:

	//! Style of the new list.
	const listlayoutstyle_impl &list_style;

	//! Single or multiple selection

	//! This may be set to:
	//!
	//! - \c INSERT_LIBX_NAMESPACE::w::single_selection_type (default):
	//!
	//! - \c INSERT_LIBX_NAMESPACE::w::single_optional_selection_type:
	//!
	//! - \c INSERT_LIBX_NAMESPACE::w::multiple_selection_type
	//!
	//! \see single_selection_type
	//! \see single_optional_selection_type
	//! \see multiple_selection_type

	list_selection_type_cb_t selection_type;

	//! Callback when list selection changes.

	//! Invoked each time a list item is selected or unselected.
	//!
	//! This callback cannot strongly capture the list display element
	//! or any of its parent display elements.

	list_selection_changed_cb_t selection_changed;

	//! The height of the list.

	//! The height may be specified either as a number of rows, this
	//! sets the actual size of the list element based on the number
	//! of rows and the default list fon't height.
	//!
	//! Alternatively the list's height may be specified as a dim_axis_arg.
	//!
	//! The default height is four rows.

	std::variant<size_t, dim_axis_arg> height;

	//! Set the new list's height as completely variable.

	//! This is often used when creating a selection list
	//! inside a \ref panelayoutmanager "pane".

	inline void variable_height()
	{
		height=dim_axis_arg{0, 0};
	}

	//! Number of columns in the list

	//! This is not the number of characters, but the number of
	//! elements in each list item. The default is 1.
	size_t columns;

	//! Relative width of columns.

	//! If the selection list is filled horizontally, inside its parent
	//! grid layout manager, the additional space goes to these columns.
	std::unordered_map<size_t, int> requested_col_widths;

	//! Alignment of all columns.
	std::unordered_map<size_t, halign> col_alignments;

	//! What other list this list's columns get synchronized with.
	synchronized_axis synchronized_columns;

	//! The border around the list.
	border_arg list_border;

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
	color_arg background_color;

	//! Selected color

	//! Used only by a \c INSERT_LIBX_NAMESPACE::w::highlighted_list.
	color_arg selected_color;

	//! Highlighted color
	color_arg highlighted_color;

	//! Currently selected color
	color_arg current_color;

	//! List font
	font_arg list_font;

	//! Constructor
	new_listlayoutmanager(const listlayoutstyle_impl &list_style
			      =highlighted_list)
		LIBCXX_PUBLIC;

	//! Destructor
	~new_listlayoutmanager() LIBCXX_PUBLIC;

	//! \internal
	//! Create the focusable container.

	focusable_container create(const container_impl &)
		const override;
};

//! Lock the list layout manager.

//! Blocks modifications to the contents of the list.

class list_lock : public listimpl_info_lock_t {

public:

	const const_listlayoutmanager layout_manager;

	//! Disambiguating constructor
	template<typename ref_or_ptr, typename=
		 typename refptr_traits<ref_or_ptr>::obj_t>
	inline list_lock(const ref_or_ptr &manager)
		: list_lock(*manager)
	{
	}

	//! Constructor
	list_lock(const listlayoutmanagerObj &manager);

	//! Destructor
	~list_lock();
};

//! New list item parameter.

//! Various list layout manager methods that
//! create new list items specified as a \c std::vector of this class.
//!
//! This class (inherits from) a \c std::variant that takes several possible
//! values.
//!
//! \see listlayoutmanager.

struct list_item_param : public list_item_param_base {

	using list_item_param_base::list_item_param_base;

	//! Base variant class.

	typedef list_item_param_base variant_t;

	//! Constructor overload for a \ref text_param "INSERT_LIBX_NAMESPACE::w::text_param"

	//! Helps with constructing the variant class as a
	//! \ref text_param "text_param".

	template<typename Arg1, typename ...Args,
		 typename=std::void_t<text_param_constructible
				      <Arg1, Args...>>>
	list_item_param(Arg1 &&arg1,
			Args && ...args)
		: list_item_param_base{text_param{std::forward<Arg1>(arg1),
				std::forward<Args>(args)...}}
	{
	}

	//! Constructor overload for a \ref list_item_status_change_callback "INSERT_LIBX_NAMESPACE::w::list_item_status_change_callback"

	//! Helps with constructing the variant class as a
	//! \ref list_item_status_change_callback "list_item_status_change_callback".

	template<typename Arg1, typename=
		 std::enable_if<is_list_item_status_change_callback<Arg1>
				::value>>
	list_item_param(Arg1 &&arg1)
		: list_item_param_base{list_item_status_change_callback{
			std::forward<Arg1>(arg1)}}
	{
	}

	//! Destructor
	~list_item_param();

	//! Copy constructor
	list_item_param(const list_item_param &);

	//! Move constructor
	list_item_param(list_item_param &&);

	//! Assignment operator
	list_item_param &operator=(const list_item_param &);

	//! Move assignment operator
	list_item_param &operator=(list_item_param &&);
};

LIBCXXW_NAMESPACE_END

#endif
