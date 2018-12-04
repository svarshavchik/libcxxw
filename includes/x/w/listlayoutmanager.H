/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_listlayoutmanager_h
#define x_w_listlayoutmanager_h

#include <x/w/listlayoutmanagerfwd.H>
#include <x/w/listlayoutmanagerobj.H>
#include <x/w/gridlayoutmanagerfwd.H>
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
#include <x/w/border_arg.H>
#include <x/w/synchronized_axisfwd.H>
#include <x/w/factoryfwd.H>
#include <x/ref.H>
#include <x/refptr_traits.H>
#include <x/functionalrefptr.H>
#include <string>
#include <variant>
#include <tuple>
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

class new_listlayoutmanager : public new_focusable_layoutmanager {

 public:

	//! Style of the new list.
	const listlayoutstyle_impl &list_style;

	//! Single or multiple selections

	//! This specifies whether only one list item may be visually selected
	//! at any time, or multiple list items (or none).
	//!
	//! - \c INSERT_LIBX_NAMESPACE::w::single_selection_type (default):
	//!
	//! - \c INSERT_LIBX_NAMESPACE::w::single_optional_selection_type:
	//!
	//! - \c INSERT_LIBX_NAMESPACE::w::multiple_selection_type
	//!
	//! - \c INSERT_LIBX_NAMESPACE::w::no_selection_type
	//!
	//! \see single_selection_type
	//! \see single_optional_selection_type
	//! \see multiple_selection_type
	//! \see no_selection_type

	list_selection_type_cb_t selection_type;

	//! Callback when list selection changes.

	//! Invoked each time a list item is selected or unselected.
	//!
	//! This callback cannot strongly capture the list display element
	//! or any of its parent display elements.

	list_selection_changed_cb_t selection_changed;

private:
	//! The height of the list.

	std::variant<std::tuple<size_t, size_t>, dim_axis_arg> height_value;
public:
	//! The list's height is set to this number of rows.

	inline void height(size_t rows)
	{
		height_value=std::tuple{rows, rows};
	}

	//! The list's height is between these many rows.

	//! Minimum list height, and maximum list height.
	//!
	//! \ref scrollbar_visibility "vertical_scrollbar" should be set
	//! to either \c scrollbar_visibility::always or
	//! scrollbar_visibility::automatic_reserved when specifying a varying
	//! number of rows, to avoid display flicker.

	inline void height(size_t min_rows, size_t max_rows)
	{
		height_value=std::tuple{min_rows, max_rows};
	}

	//! Specify the list's height as a dim_axis_arg

	inline void height(const dim_axis_arg &arg)
	{
		height_value=arg;
	}

	//! Set the new list's height as completely variable.

	//! This is often used when creating a selection list
	//! inside a \ref panelayoutmanager "pane". This is a shortcut for
	//! specifying an x::w::dim_axis_arg that specifies a minimum height
	//! of 0, and no maximum height.

	inline void variable_height()
	{
		height_value=dim_axis_arg{0, 0};
	}

private:
	//! List's width.

	//! If not set, the list is always wide enough for its widest element.

	std::optional<dim_axis_arg> width_value;
public:

	//! By default, the list's width is determined by its widest element.
	inline void default_width()
	{
		width_value.reset();
	}

	//! Specify an explicit list width.

	inline void width(const dim_axis_arg &arg)
	{
		width_value=arg;
	}

	//! Set the new list's width as completely variable.
	inline void variable_width()
	{
		width(dim_axis_arg{0, 0});
	}

	//! Number of columns in the list

	//! This is not the number of characters, but the number of
	//! elements in each list item. The default is 1.
	size_t columns;

	//! Relative width of columns.

	//! If the selection list is filled horizontally, inside its parent
	//! grid layout manager, the additional space goes to these columns.
	//!
	//! This specifies the request width of a column as a percentage of
	//! the total width of the list, in the range 0-100.
	std::unordered_map<size_t, int> requested_col_widths;

	//! Horizontal alignment of columns.
	std::unordered_map<size_t, halign> col_alignments;

	//! Vertical alignment of columns.
	std::unordered_map<size_t, valign> row_alignments;

	//! Optional borders between columns

	//! Draw these borders between the columns. columns_borders[1]
	//! specifies the border between the 1st and the 2nd column
	//! (column #0 and column #1).
	//!
	//! Map keys must be between 1 and (columns-1).
	//!
	//! The focus border gets drawn around the list, and there should
	//! always be a focus border that visually connects with the column
	//! borders, see focusoff_border, below.

	std::unordered_map<size_t, border_arg> column_borders;

	//! Minimum column widths, specified in millimeters

	//! Mostly for the benefit of the table layout manager. If not specified
	//! the default gets computed based on list contents.
	//!
	//! \see tablelayoutmanager

	std::unordered_map<size_t, double> minimum_column_widths;

	//! What other list this list's columns get synchronized with.

	//! Lists created using the same synchronized_columns will have
	//! their columns' widths equalized. Positioning the lists above
	//! each results in both lists's columns lining up with each other.
	//!
	//! For this to work correctly:
	//!
	//! - Both lists must be sized to the same width by their container.
	//! Using the grid layout manager and using halign::fill is the
	//! simplest way to do that.
	//!
	//! - both lists must have the same number of columns.
	//!
	//! - both lists must have same borders. This includes the list_border,
	//! focusoff_border, focuson_border, and column_borders. Or, at least,
	//! the borders must have the same width.
	//!
	//! - Both lists have the same h_padding.
	//!
	//! It is also possible to, very carefully, synchronize the columns
	//! between a grid and the list, by using the same
	//! synchronized_columns with a
	//! \ref new_gridlayoutmanager "new_gridlayoutmanager".
	//!
	//! This works correctly only if:
	//!
	//! - both this list and the other list or grid have the same number
	//! of columns
	//!
	//! - all borders are the same.
	//!
	//! - Their left and right paddings are the same as v_padding.
	//!
	//! synchronized_columns ignored if this list is part of the table, and
	//! this is overridden by
	//! \ref new_tablelayoutmanager "INSERT_LIBX_NAMESPACE::w::new_tablelayoutmanager"
	//! which uses this to implement the table's header.

	synchronized_axis synchronized_columns;

	//! The border around the list.
	border_arg list_border;

	//! Inner border around the list when the list does not have input focus

	//! Note that if the list has column_borders, they connect to the
	//! input focus frame, so this should be visible. The default
	//! is "listfocusoff_border", which is empty, no border drawn around
	//! the list when it has no focus. This border is the same size as
	//! "listfocuson_border".
	//!
	//! Use visible_focusoff_border() to install the default theme
	//! focusoff border.

	border_arg focusoff_border;

	//! Set the focusoff_border to a visible border.

	//! This gets called automatically if this is a superclass of
	//! \ref new_tablelayoutmanager "INSERT_LIBX_NAMESPACE::w::new_tablelayoutmanager"
	//! because it typically has visible borders. If a plain list has
	//! borders, this should be invoked to install a lesser intensity
	//~ border that's
	//! visible, but does not indicate input focus, and provides a visual
	//! connection with column_borders.

	void visible_focusoff_border();

	//! Inner border around the list when the list has input focus

	//! Border should be the same size as the focusoff_border
	border_arg focuson_border;

	//! Padding above and below each list item
	dim_arg v_padding;

	//! Default padding before and after each list item.
	dim_arg h_padding;

	//! When using hierarchical indentations, indentation increment.
	dim_arg indent;

	//! Optionally-specified paddings for each cell

	//! Map keyed by column number. Value is {left_padding, right_padding}
	//! for that column. h_padding is the default, if not specified.

	std::unordered_map<size_t, std::tuple<dim_arg, dim_arg>> lr_paddings;

	//! Visibility of the horizontal scrollbar
	scrollbar_visibility horizontal_scrollbar;

	//! Visibility of the vertical scrollbar

	//! If the list's height, height() is set to a varying number of rows,
	//! the default "automatic" visibility always gets replaced with
	//! "automatic_reserved", to avoid suboptimal behavior.
	scrollbar_visibility vertical_scrollbar;

	//! Background color
	color_arg background_color;

	//! Selected color

	//! Used only by a \c INSERT_LIBX_NAMESPACE::w::highlighted_list.
	color_arg selected_color;

	//! Highlighted color

	//! Item's color when the pointer button #1 is pressed on top of it.
	color_arg highlighted_color;

	//! Currently selected color

	//! Item's color when the pointer is hovering on top of it, without
	//! the button being pressed.
	color_arg current_color;

	//! Default list font
	font_arg list_font;

	//! Horizontal separator line
	border_arg list_separator_border;

	//! Constructor
	new_listlayoutmanager(const listlayoutstyle_impl &list_style
			      =highlighted_list)
		LIBCXX_PUBLIC;

	//! Destructor
	~new_listlayoutmanager() LIBCXX_PUBLIC;

	//! Create the focusable container.

	//! \internal
	focusable_container create(const container_impl &)
		const override;

	struct LIBCXX_HIDDEN table_create_info;

	struct LIBCXX_HIDDEN list_create_info;

 protected:
	//! Internal implementation
	focusable_container create_impl(const container_impl &,
					const synchronized_axis &,
					table_create_info *,
					const list_create_info &)
		const;
 public:
	//! Implemented in \ref new_tablelayoutmanager "new_tablelayoutmanager".

	//! \internal
	virtual void created_list_container(const gridlayoutmanager &,
					    table_create_info *) const;

	//! Implemented in \ref new_tablelayoutmanager "new_tablelayoutmanager".

	//! \internal

	virtual void create_table_header_row(const gridlayoutmanager &,
					     table_create_info *) const;
};

//! Lock the list layout manager.

//! Blocks modifications to the contents of the list.

class const_list_lock : public listimpl_info_lock_t {

public:

	// My layout manager.
	const const_listlayoutmanager locked_layoutmanager;

	//! Disambiguating constructor
	template<typename ref_or_ptr, typename=
		 decltype(static_cast<const listlayoutmanagerObj &>
			  (std::declval<typename refptr_traits<ref_or_ptr>
			   ::obj_t &>()))>
	inline const_list_lock(const ref_or_ptr &manager)
		: const_list_lock(*manager)
	{
	}

	//! Constructor
	const_list_lock(const listlayoutmanagerObj &manager);

	//! Destructor
	~const_list_lock();
};

//! Lock the list layout manager

//! Blocks modifications to the contents of the list.

class list_lock : public const_list_lock {

public:
	// My layout manager.
	const listlayoutmanager locked_layoutmanager;

	//! Disambiguating constructor
	template<typename ref_or_ptr, typename=
		 decltype(static_cast<listlayoutmanagerObj &>
			  (std::declval<typename refptr_traits<ref_or_ptr>
			   ::obj_t &>()))>
	inline list_lock(const ref_or_ptr &manager)
		: list_lock(*manager)
	{
	}

	//! Constructor
	list_lock(listlayoutmanagerObj &manager);

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
