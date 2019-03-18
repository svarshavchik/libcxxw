/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_listlayoutmanager_appearance_h
#define x_w_listlayoutmanager_appearance_h

#include <x/w/border_arg.H>
#include <x/w/dim_arg.H>
#include <x/w/font_arg.H>
#include <x/w/rgb.H>

LIBCXXW_NAMESPACE_START

//! Appearance of a selection list

//! Inherited by
//! \ref new_listlayoutmanager "INSERT_LIBX_NAMESPACE::w::new_listlayoutmanager"
//! and other layout manager that inherit the functionality of the list
//! layout manager, such as the combo-box layout managers.

struct listlayoutmanager_appearance {

	//! Constructor
	listlayoutmanager_appearance();

	//! Destructor
	~listlayoutmanager_appearance();

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

	//! Default list font
	font_arg list_font;

	//! Horizontal separator line
	border_arg list_separator_border;

	//! Cached singleton instance of the default list appearance.

	static const listlayoutmanager_appearance &standard();
};

LIBCXXW_NAMESPACE_END

#endif
