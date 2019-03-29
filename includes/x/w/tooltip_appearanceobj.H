/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_tooltip_appearanceobj_h
#define x_w_tooltip_appearanceobj_h

#include <x/w/tooltip_appearancefwd.H>
#include <x/w/border_arg.H>
#include <x/w/font_arg.H>
#include <x/w/dim_arg.H>
#include <x/w/rgb.H>
#include <x/functional.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START


//! Configuration for a tooltip border

//! A tooltip border, and inner padding.
//!
//! \see tooltip_appearance_properties

struct tooltip_border {

	//! Border
	border_arg border;

	//! Additional padding
	dim_arg hpad;

	//! Additional padding
	dim_arg vpad;

	//! Initialize border, hpad, and vpad, using theme naming convention.

	//! Mostly used internally by the library.
	//! Sets "border", and the paddings to
	//! "border_padding_h" and "border_padding_v";

	void set_theme_border(const std::string &border);
};

//! Actual properties inherited by \ref tooltip_appearance "INSERT_LIBX_NAMESPACE::w::tooltip_appearance".

struct tooltip_appearance_properties {

	//! Constructor
	tooltip_appearance_properties();

	//! Destructor
	~tooltip_appearance_properties();

	//! Border for the new tooltip

	//! Used when the display has an alpha channel.
	//! Typically a slightly rounded border.

	tooltip_border alpha_border;

	//! Border for the new tooltip

	//! Used when the display does not have an alpha channel.
	//! Typically a square border.

	tooltip_border nonalpha_border;

	//! Background color
	color_arg tooltip_background_color;

	//! Default font for the tooltip text
	font_arg label_font;

	//! Default color for the tooltip text
	color_arg label_foreground_color;

	//! Tooltip's shade color

	//! The color the tooltip gets shaded with, when it's blocked by
	//! a modal dialog (unlikely).
	color_arg modal_shade_color;

	//! Tooltip location, to the right of the pointer position.

	//! Regular tooltips only, ignored for static tooltips
	dim_arg tooltip_x_offset;

	//! Tooltip location, above the pointer position.

	//! Regular tooltips only, ignored for static tooltips
	dim_arg tooltip_y_offset;
};

//! Implementation object.

//! \see tooltip_appearance

class tooltip_appearanceObj: public tooltip_appearance_properties,
				 virtual public obj {

public:
	//! Constructor
	tooltip_appearanceObj();

	//! Destructor
	~tooltip_appearanceObj();

	//! Copy constructor
	tooltip_appearanceObj(const tooltip_appearanceObj &);

	//! Create a copy of this object.
	template<typename F> inline const_tooltip_appearance modify(F &&f) const
	{
		return do_modify(make_function<void(const tooltip_appearance &)>
				 (std::forward<F>(f)));
	}

	//! Type-erased modify().
	const_tooltip_appearance do_modify(const function
					<void(const tooltip_appearance &)> &)
		const;
};

LIBCXXW_NAMESPACE_END

#endif
