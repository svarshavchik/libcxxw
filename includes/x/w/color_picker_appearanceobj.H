/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_color_picker_appearanceobj_h
#define x_w_color_picker_appearanceobj_h

#include <x/w/color_picker_appearancefwd.H>
#include <x/w/element_popup_appearancefwd.H>
#include <x/w/input_field_appearancefwd.H>
#include <x/w/button_appearancefwd.H>
#include <x/w/border_arg.H>
#include <x/w/font_arg.H>
#include <x/w/dim_arg.H>
#include <x/w/rgb.H>
#include <x/functional.H>
#include <x/w/appearanceobj.H>
#include <x/w/color_picker_appearance.inc.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see color_picker_appearance

class color_picker_appearanceObj: public color_picker_appearance_properties,
				  public appearanceObj {

public:
	//! Constructor
	color_picker_appearanceObj();

	//! Destructor
	~color_picker_appearanceObj();

	//! Copy constructor
	color_picker_appearanceObj(const color_picker_appearanceObj &);

	//! Create a copy of this object.
	template<typename F> inline const_color_picker_appearance modify(F &&f) const
	{
		return do_modify(make_function<void(const color_picker_appearance &)>
				 (std::forward<F>(f)));
	}

	//! Type-erased modify().
	const_color_picker_appearance do_modify(const function
					<void(const color_picker_appearance &)> &)
		const;
};

LIBCXXW_NAMESPACE_END

#endif
