/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_element_popup_appearanceobj_h
#define x_w_element_popup_appearanceobj_h

#include <x/w/element_popup_appearancefwd.H>
#include <x/w/scrollbar_appearancefwd.H>
#include <x/w/generic_window_appearancefwd.H>
#include <x/w/border_arg.H>
#include <x/w/font_arg.H>
#include <x/w/dim_arg.H>
#include <x/w/rgb.H>
#include <x/functional.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Actual properties inherited by \ref element_popup_appearance "INSERT_LIBX_NAMESPACE::w::element_popup_appearance".

struct element_popup_appearance_properties {

	//! Constructor
	element_popup_appearance_properties();

	//! Destructor
	~element_popup_appearance_properties();

	//! Border around the display element
	border_arg element_border;

	//! Popup border
	border_arg popup_border;

	//! Popup background color
	color_arg popup_background_color;

	//! Popup background color
	color_arg popup_scrollbars_background_color;

	//! Button focus border.
	border_arg button_focusoff_border;

	//! Button focus border.
	border_arg button_focuson_border;

	//! Generic appearance of the top-level window that makes up the popup.
	const_generic_window_appearance toplevel_appearance;

	//! Popup button image
	std::string button_image1;

	//! Popup button image
	std::string button_image2;

	//! Horizontal scrollbar appearance
	const_scrollbar_appearance horizontal_scrollbar;

	//! Vertical scrollbar appearance
	const_scrollbar_appearance vertical_scrollbar;
};

//! Implementation object.

//! \see element_popup_appearance

class element_popup_appearanceObj: public element_popup_appearance_properties,
				 virtual public obj {

public:
	//! Constructor
	element_popup_appearanceObj();

	//! Destructor
	~element_popup_appearanceObj();

	//! Copy constructor
	element_popup_appearanceObj(const element_popup_appearanceObj &);

	//! Create a copy of this object.
	template<typename F> inline const_element_popup_appearance modify(F &&f) const
	{
		return do_modify(make_function<void(const element_popup_appearance &)>
				 (std::forward<F>(f)));
	}

	//! Type-erased modify().
	const_element_popup_appearance do_modify(const function
					<void(const element_popup_appearance &)> &)
		const;
};

LIBCXXW_NAMESPACE_END

#endif
