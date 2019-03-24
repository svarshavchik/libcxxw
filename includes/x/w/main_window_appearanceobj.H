/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_main_window_appearanceobj_h
#define x_w_main_window_appearanceobj_h

#include <x/w/main_window_appearancefwd.H>
#include <x/w/scrollbar_appearancefwd.H>
#include <x/w/border_arg.H>
#include <x/w/rgb.H>
#include <x/w/font_arg.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Actual properties inherited by \ref main_window_appearance "INSERT_LIBX_NAMESPACE::w::main_window_appearance".

struct main_window_appearance_properties {

	//! Constructor
	main_window_appearance_properties();

	//! Destructor
	~main_window_appearance_properties();

	//! Window's horizontal scrollbar appearance
	const_scrollbar_appearance horizontal_scrollbar;

	//! Window's vertical scrollbar appearance
	const_scrollbar_appearance vertical_scrollbar;

	//! Main window's background color.
	color_arg background_color;

	//! Main window's menu bar's background color
	color_arg menubar_background_color;

	//! Default font for labels
	font_arg label_font;

	//! Default color (mostly for labels)
	color_arg label_foreground_color;

	//! Window's shade color

	//! The color the window gets shaded with, when it's blocked by
	//! a modal dialog.
	color_arg modal_shade_color;

	//! Menu button has input or pointer focus.
	color_arg menu_highlighted_color;

	//! Menu button is clicked.
	color_arg menu_clicked_color;

	//! Menu button does not have keyboard focus
	border_arg menu_inputfocusoff_border;

	//! Menu button has keyboard focus
	border_arg menu_inputfocuson_border;

	//! Border between the menu bar and the main window.
	border_arg menubar_border;

};

//! Implementation object.

//! \see main_window_appearance

class main_window_appearanceObj: public main_window_appearance_properties,
				 virtual public obj {

public:
	//! Constructor
	main_window_appearanceObj();

	//! Destructor
	~main_window_appearanceObj();

	//! Copy constructor
	main_window_appearanceObj(const main_window_appearanceObj &);

	//! Create a copy of this object.
	main_window_appearance clone() const;
};

LIBCXXW_NAMESPACE_END

#endif
