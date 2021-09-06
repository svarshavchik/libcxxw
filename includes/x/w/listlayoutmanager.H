/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_listlayoutmanager_h
#define x_w_listlayoutmanager_h

#include <x/w/listlayoutmanagerfwd.H>
#include <x/w/listlayoutmanagerobj.H>
#include <x/w/list_appearance.H>
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
#include <utility>
#include <tuple>
#include <unordered_map>

LIBCXXW_NAMESPACE_START

//! A superclass of \ref submenu "submenu" that holds the creator callback.

//! Makes it possible to use aggregate initialization with submenu, and
//! default-construct the optional custom submenu appearance.

struct submenu_creator {

	//! Callback invoked to create the submenu.

	functionref<void (const listlayoutmanager &)> creator;
};

//! A superclass of \ref submenu "submenu" that holds the submenu's appearance.

//! Makes it possible to use aggregate initialization with submenu, and
//! default-construct the const_popup_list_appearance using only a
//! forward-declaration

struct submenu_appearance {

	//! Submenu's appearance.

	const_popup_list_appearance appearance;

	//! Default constructor

	submenu_appearance();

	//! Constructor
	submenu_appearance(const const_popup_list_appearance &appearance);

	//! Copy constructor
	submenu_appearance(const submenu_appearance &appearance);

	//! Assignment operator
	submenu_appearance &operator=(const submenu_appearance &appearance);

	//! Destructor
	~submenu_appearance();
};

//! This is a menu item entry for a sub menu.

struct submenu : submenu_creator, submenu_appearance {};

/*!

Configuration common to list and combo-box layout managers.

Common configuration settings, inherited by
\ref new_listlayoutmanager "INSERT_LIBX_NAMESPACE::w::new_listlayoutmanager",
\ref new_custom_comboboxlayoutmanager "INSERT_LIBX_NAMESPACE::w::new_custom_comboboxlayoutmanager",
\ref new_standard_comboboxlayoutmanager "INSERT_LIBX_NAMESPACE::w::new_standard_comboboxlayoutmanager", and
\ref new_editable_comboboxlayoutmanager "INSERT_LIBX_NAMESPACE::w::new_editable_comboboxlayoutmanager",

\par Synchronized lists

It is possible to use the same
\c new_list_or_combobox_layoutmanager object to create more
than one selection list or combo-box. Each individual list's width gets
automatically sized to be big enough to show the widest list item;
but using the same new listlayoutmanager object makes all individual lists
and combo-boxes have the same width, the width of the widest list or
combo-box.
This results in a tidy visual appearance when these lists or
combo-boxes appear in the same column.

This is due to the default-constructed \c synchronized_columns member.
Each selection list or combo-box constructed with the same
\ref synchronized_axis "synchronized axis object" and the same number
of columns gets sized together with all selection lists or
combo-boxes that link to the
same synchronized axis object. It is possible to constructed multiple
selection lists or combo-boxes
using different \c new_list_or_comboboxlayoutmanager objects, provided
that the synchronized axis object gets carefully copied to each one.

*/

class new_list_or_combobox_layoutmanager {

protected:
	//! List's or combo-box's width.

	//! If not set, the list or a combo-box is always wide enough
	//! for its widest content.

	std::optional<dim_axis_arg> width_value;
public:

	new_list_or_combobox_layoutmanager() LIBCXX_HIDDEN;

	//! By default, the width is determined by its widest content.

	inline void default_width()
	{
		width_value.reset();
	}

	//! Specify an explicit width.

	inline void width(const dim_axis_arg &arg)
	{
		width_value=arg;
	}

	//! Set the new width as completely variable.

	//! The parent widget can resize it, and make this list or combo-box
	//! shorter or wider than its widest content.
	inline void variable_width()
	{
		width(dim_axis_arg{0, 0});
	}

	//! Number of columns

	//! This is not the number of characters, but the number of
	//! elements in each list or a combo-box item. The default is 1.
	//!
	//! \note
	//! \ref standard_comboboxlayoutmanager "standard" and
	//! \ref editable_comboboxlayoutmanager "editable" combo-boxes
	//! support multiple columns but show only the first column's text
	//! in their label or input fields.

	size_t columns;

	//! Relative width of columns.

	//! If the list or combo-box gets filled horizontally,
	//! the additional space goes to these columns.
	//!
	//! This specifies the requested width of a column as a percentage of
	//! the total width, in the range 0-100.
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
	//! For tables:
	//!
	//! The focus border gets drawn around the entire list, and there should
	//! always be a focus border that visually connects with the column
	//! borders, see focusoff_border, below.
	//!
	//! \see tablelayoutmanager

	std::unordered_map<size_t, border_arg> column_borders;

	//! Minimum column widths, specified in millimeters

	//! Mostly for the benefit of the table layout manager. If not specified
	//! the default gets computed based on list contents.
	//!
	//! \see tablelayoutmanager

	std::unordered_map<size_t, double> minimum_column_widths;

	//! What other list these columns get synchronized with.

	//! Lists or combo-boxes
	//! created using the same \c synchronized_columns will have
	//! their columns' widths equalized. Positioning them above
	//! each other results in their columns lining up with each other.
	//!
	//! For this to work correctly:
	//!
	//! - All lists or combo-boxes
	//! must be sized to the same width by their container.
	//! Using the grid layout manager and using halign::fill is the
	//! simplest way to do that.
	//!
	//! - All lists or combo-boxes must have the same number of columns.
	//!
	//! - They must have same borders. This includes the list_border,
	//! focusoff_border, focuson_border, and column_borders. Or, at least,
	//! the borders must have the same width.
	//!
	//! - They must have the same h_padding.
	//!
	//! It is also possible to, very carefully, synchronize the columns
	//! between a grid and the list or combo-box, by using the same
	//! synchronized_columns with a
	//! \ref new_gridlayoutmanager "new_gridlayoutmanager".
	//!
	//! This works correctly only if:
	//!
	//! - both this list or combo-box and tje grid have the same number
	//! of columns
	//!
	//! - all borders are the same.
	//!
	//! - Their left and right paddings are the same as v_padding.
	//!
	//! \note
	//!
	//! The \c synchronized_columns get
	//! ignored if this list is part of the table, and
	//! this is overridden by
	//! \ref new_tablelayoutmanager "INSERT_LIBX_NAMESPACE::w::new_tablelayoutmanager"
	//! which uses this to implement the table's header.

	synchronized_axis synchronized_columns;
	//! Optionally-specified paddings for each cell

	//! Map keyed by column number. Value is {left_padding, right_padding}
	//! for that column. h_padding is the default, if not specified.

	std::unordered_map<size_t, std::tuple<dim_arg, dim_arg>> lr_paddings;
};

/*!

Create a new container with a \ref listlayoutmanager "list layout manager".

This parameter gets passed to a \ref factory "factory"'s
\c create_focusable_container()
to create a simple selection list.

The constructor takes one optional parameter, which can be:

- \c INSERT_LIBX_NAMESPACE::w::highlighted_list - selected items in
the list are displayed by a highlighted background color. This is the
default value.

- \c INSERT_LIBX_NAMESPACE::w::bulleted_list - a bullet appears next to
each selected item in the list.

The remaining fields get initialized to their default values and may be
modified prior before using \c create_focusable_container().

\par Common settings

\c INSERT_LIBX_NAMESPACE::w::new_listlayoutmanager inherits from
\ref new_list_or_combobox_layoutmanager "INSERT_LIBX_NAMESPACE::w::new_list_or_combobox_layoutmanager"
that has settings which are common with
\ref new_custom_comboboxlayoutmanager "INSERT_LIBX_NAMESPACE::w::new_custom_comboboxlayoutmanager",
\ref new_standard_comboboxlayoutmanager "INSERT_LIBX_NAMESPACE::w::new_standard_comboboxlayoutmanager", and
\ref new_editable_comboboxlayoutmanager "INSERT_LIBX_NAMESPACE::w::new_editable_comboboxlayoutmanager",

*/

class new_listlayoutmanager : public new_focusable_layoutmanager,
			      public new_list_or_combobox_layoutmanager {


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
	//! This callback cannot strongly capture the list widget
	//! or any of its parent widgets.

	list_selection_changed_cb_t selection_changed;

	//! Callback when currently highlighted list item changes.

	//! This callback cannot strongly capture the list widget
	//! or any of its parent widgets.

	list_item_status_change_callbackptr current_list_item_changed;

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

	//! Visibility of the horizontal scrollbar
	scrollbar_visibility horizontal_scrollbar;

	//! Visibility of the vertical scrollbar

	//! If the list's height, height() is set to a varying number of rows,
	//! the default "automatic" visibility always gets replaced with
	//! "automatic_reserved", to avoid suboptimal behavior.
	scrollbar_visibility vertical_scrollbar;

	//! This list's appearance.
	const_list_appearance appearance;

	//! Set appearance of this list in a pane.

	//! Installs INSERT_LIBX_NAMESPACE::w::list_appearance::base::list_pane_theme().
	//! Overridden by the table layout manager, that sets a slightly
	//! different theme.

	virtual void set_pane_theme() LIBCXX_HIDDEN;

	//! Constructor
	new_listlayoutmanager(const listlayoutstyle_impl &list_style
			      =highlighted_list)
		LIBCXX_PUBLIC;

	//! Destructor
	~new_listlayoutmanager() LIBCXX_PUBLIC;

	//! Configure this list or a table for a pane

	//! Tailors the list for optimum apperance in a
	//! \ref panelayoutmanager "pane container".
	//!
	//! Installs a new appearance object, sets variable_height().
	//! If the synchronized_flag is set, sets the scrollbar visibility
	//! to "always".

	void configure_for_pane(bool synchronized=false);

	//! Create the focusable container.

	//! \internal
	focusable_container create(const container_impl &parent,
				   const function<void
				   (const focusable_container &)>
				   &creator)
		const override LIBCXX_HIDDEN;

	struct LIBCXX_HIDDEN table_create_info;

	struct LIBCXX_HIDDEN list_create_info;

 protected:
	//! Internal implementation
	focusable_container create_impl(const container_impl &,
					const synchronized_axis &,
					table_create_info *,
					const list_create_info &,
					const function<void
					(const focusable_container &)>
					&creator)
		const;
 public:
	//! Implemented in \ref new_tablelayoutmanager "new_tablelayoutmanager".

	//! \internal
	virtual void created_list_container(const container_impl &,
					    table_create_info *) const
		LIBCXX_HIDDEN;

	//! Implemented in \ref new_tablelayoutmanager "new_tablelayoutmanager".

	//! \internal

	virtual void create_table_header_row(const gridlayoutmanager &,
					     table_create_info *) const
		LIBCXX_HIDDEN;
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

	template<typename Arg1, typename ...Args>
	list_item_param(Arg1 &&arg1,
			Args && ...args)
		requires text_param_constructible<Arg1, Args...>
		: list_item_param_base{std::in_place_type_t<text_param>{},
				       std::forward<Arg1>(arg1),
				       std::forward<Args>(args)...}
	{
	}

	//! Constructor overload for a \ref list_item_status_change_callback "INSERT_LIBX_NAMESPACE::w::list_item_status_change_callback"

	//! Helps with constructing the variant class as a
	//! \ref list_item_status_change_callback "list_item_status_change_callback".
	template<is_list_item_status_change_callback Arg1>
	list_item_param(Arg1 &&arg1)
		: list_item_param_base{std::in_place_type_t<
				       list_item_status_change_callback>(),
				       std::forward<Arg1>(arg1)}
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
