/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_focus_border_appearance_h
#define x_w_focus_border_appearance_h

#include <x/w/focus_border_appearancefwd.H>
#include <x/w/focus_border_appearanceobj.H>
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

//! Base class for a \ref focus_border_appearance "focus_border appearance" specification.

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::focus_border_appearance::base.

class focus_border_appearance_base : public ptrref_base {

public:

	//! Default focus border, for buttons.

	static const const_focus_border_appearance &theme();

	//! Focus border for menu buttons in the main window's menu bar.

	static const const_focus_border_appearance &menu_theme();

	//! Smaller input focus border, for checkboxes and radio-buttons.

	static const const_focus_border_appearance &thin_theme();

	//! Smaller input focus border, for checkboxes and radio-buttons.

	//! Visible border when there's no focus.
	static const const_focus_border_appearance &visible_thin_theme();

	//! Focus border, for \ref panelayoutmanager "pane sliders".
	static const const_focus_border_appearance &slider_theme();

	//! Focus border for combo-box buttons.

	static const const_focus_border_appearance &combobox_button_theme();

	//! Focus border for \ref input_field "input fields".

	static const const_focus_border_appearance &input_field_theme();

	//! Focus border for \ref listlayoutmanager "lists".
	static const const_focus_border_appearance &list_theme();

	//! Always visible focus border for \ref listlayoutmanager "lists".
	static const const_focus_border_appearance &visible_list_theme();

	//! Focus border for buttons that open an attached popup (various).
	static const const_focus_border_appearance &attached_button_theme();

	//! Focus border for \ref scrollbar "scroll-bars".
	static const const_focus_border_appearance &scrollbar_theme();

	//! No focus border.

	//! Both focus off and on borders are 0-width borders, effectively.

	static const const_focus_border_appearance &none_theme();
};

LIBCXXW_NAMESPACE_END

#endif
