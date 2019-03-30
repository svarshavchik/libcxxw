/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_generic_window_appearanceobj_h
#define x_w_generic_window_appearanceobj_h

#include <x/w/generic_window_appearancefwd.H>
#include <x/w/border_arg.H>
#include <x/w/font_arg.H>
#include <x/w/dim_arg.H>
#include <x/w/rgb.H>
#include <x/functional.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Actual properties inherited by \ref generic_window_appearance "INSERT_LIBX_NAMESPACE::w::generic_window_appearance".

struct generic_window_appearance_properties {

	//! Constructor
	generic_window_appearance_properties();

	//! Destructor
	~generic_window_appearance_properties();

	//! Default font
	font_arg label_font;

	//! Default color
	color_arg label_color;

	//! Shaded color

	color_arg modal_shade_color;

	//! Disabled mask

	//! Bitmask image for dithering disabled display elements

	std::string disabled_mask;

	//! "Wait" cursor image
	std::string wait_cursor;
};

//! Implementation object.

//! \see generic_window_appearance

class generic_window_appearanceObj: public generic_window_appearance_properties,
				 virtual public obj {

public:
	//! Constructor
	generic_window_appearanceObj();

	//! Destructor
	~generic_window_appearanceObj();

	//! Copy constructor
	generic_window_appearanceObj(const generic_window_appearanceObj &);

	//! Create a copy of this object.
	template<typename F> inline const_generic_window_appearance modify(F &&f) const
	{
		return do_modify(make_function<void(const generic_window_appearance &)>
				 (std::forward<F>(f)));
	}

	//! Type-erased modify().
	const_generic_window_appearance do_modify(const function
					<void(const generic_window_appearance &)> &)
		const;
};

LIBCXXW_NAMESPACE_END

#endif
