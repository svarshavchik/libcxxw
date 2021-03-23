/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_tooltip_border_appearanceobj_h
#define x_w_tooltip_border_appearanceobj_h

#include <x/w/tooltip_border_appearancefwd.H>
#include <x/w/border_arg.H>
#include <x/w/font_arg.H>
#include <x/w/dim_arg.H>
#include <x/w/rgb.H>
#include <x/functional.H>
#include <x/w/appearanceobj.H>
#include <x/w/tooltip_border_appearance.inc.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see tooltip_border_appearance

class tooltip_border_appearanceObj: public tooltip_border_appearance_properties,
				    public appearanceObj {

public:
	//! Constructor
	tooltip_border_appearanceObj();

	//! Destructor
	~tooltip_border_appearanceObj();

	//! Copy constructor
	tooltip_border_appearanceObj(const tooltip_border_appearanceObj &);

	//! Create a copy of this object.
	template<typename F> inline const_tooltip_border_appearance modify(F &&f) const
	{
		return do_modify(make_function<void(const tooltip_border_appearance &)>
				 (std::forward<F>(f)));
	}

	//! Type-erased modify().
	const_tooltip_border_appearance do_modify(const function
					<void(const tooltip_border_appearance &)> &)
		const;

	//! Initialize border, hpad, and vpad, using theme naming convention.

	//! Mostly used internally by the library.
	//! Sets "border", and the paddings to
	//! "border_padding_h" and "border_padding_v";

	void set_theme_border(const std::string &border);
};

LIBCXXW_NAMESPACE_END

#endif
