/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_list_appearanceobj_h
#define x_w_list_appearanceobj_h

#include <x/w/list_appearancefwd.H>
#include <x/w/generic_window_appearancefwd.H>
#include <x/w/border_arg.H>
#include <x/w/font_arg.H>
#include <x/w/rgb.H>
#include <x/w/dim_arg.H>
#include <x/w/scrollbar_appearancefwd.H>
#include <x/functional.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! All properties inherited by a \ref list_appearance "list appearance object".

struct list_appearance_properties {

	//! Constructor
	list_appearance_properties();

	//! Destructor
	~list_appearance_properties();

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

	//! Unmarked list icon

	//! In a bulleted list, the icon next to an unmarked item. This
	//! should be a transparent image that's the same size as the
	//! marked icon

	std::string unmarked_icon;

	//! Marked list icon

	//! Indicates that the item is selected, in a bulleted list.
	std::string marked_icon;

	//! Submenu indication icon

	//! Indicates that the menu item opens a submenu
	std::string submenu_icon;

	//! List contents appearance

	//! The label_font and label_color control the default font and color
	//! of the items in the list. Coincidentally, in combo-boxes this
	//! also controls the popup window's appearance.

	const_generic_window_appearance contents_appearance;

	//! Default shortcut font color
	color_arg shortcut_foreground_color;

	//! Default shortcut font
	font_arg shortcut_font;

	//! Horizontal separator line
	border_arg list_separator_border;

	//! Horizontal scrollbar appearance
	const_scrollbar_appearance horizontal_scrollbar;

	//! Vertical scrollbar appearance
	const_scrollbar_appearance vertical_scrollbar;
};

//! Implementation object.

//! \see list_appearance

class list_appearanceObj: public list_appearance_properties,
			  virtual public obj {

public:
	//! Constructor
	list_appearanceObj();

	//! Destructor
	~list_appearanceObj();

	//! Copy constructor
	list_appearanceObj(const list_appearanceObj &);

	//! Clone this object.

	template<typename F> inline const_list_appearance modify(F &&f) const
	{
		return do_modify(make_function<void(const list_appearance &)>
				 (std::forward<F>(f)));
	}

	//! Type-erased modify().
	const_list_appearance
	do_modify(const function<void(const list_appearance &)> &) const;
};

LIBCXXW_NAMESPACE_END

#endif