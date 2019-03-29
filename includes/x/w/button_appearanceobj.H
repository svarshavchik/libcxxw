/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_button_appearanceobj_h
#define x_w_button_appearanceobj_h

#include <x/w/button_appearancefwd.H>
#include <x/w/border_arg.H>
#include <x/w/font_arg.H>
#include <x/w/dim_arg.H>
#include <x/w/rgb.H>
#include <x/functional.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Actual properties inherited by \ref button_appearance "INSERT_LIBX_NAMESPACE::w::button_appearance".

struct button_appearance_properties {

	//! Constructor
	button_appearance_properties();

	//! Destructor
	~button_appearance_properties();

	//! Default font for any text in the button
	font_arg button_font;

	//! Normal button background color
	color_arg normal_color;

	//! Background color when the button has input focus.
	color_arg selected_color;

	//! Background color when the button is activated
	color_arg active_color;

	//! Button's border.
	border_arg left_border;

	//! Button's border.
	border_arg right_border;

	//! Button's border.
	border_arg top_border;

	//! Button's border.
	border_arg bottom_border;

	//! Inner border when the button does not have input focus
	border_arg inputfocusoff_border;

	//! Inner border when the button has input focus
	border_arg inputfocuson_border;
};

//! Implementation object.

//! \see button_appearance

class button_appearanceObj: public button_appearance_properties,
				 virtual public obj {

public:
	//! Constructor
	button_appearanceObj();

	//! Destructor
	~button_appearanceObj();

	//! Copy constructor
	button_appearanceObj(const button_appearanceObj &);

	//! Create a copy of this object.
	template<typename F> inline const_button_appearance modify(F &&f) const
	{
		return do_modify(make_function<void(const button_appearance &)>
				 (std::forward<F>(f)));
	}

	//! Type-erased modify().
	const_button_appearance do_modify(const function
					<void(const button_appearance &)> &)
		const;
};

LIBCXXW_NAMESPACE_END

#endif
