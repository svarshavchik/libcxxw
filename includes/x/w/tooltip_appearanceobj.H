/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_tooltip_appearanceobj_h
#define x_w_tooltip_appearanceobj_h

#include <x/w/tooltip_appearancefwd.H>
#include <x/w/tooltip_border_appearancefwd.H>
#include <x/w/generic_window_appearancefwd.H>
#include <x/w/border_arg.H>
#include <x/w/font_arg.H>
#include <x/w/dim_arg.H>
#include <x/w/rgb.H>
#include <x/functional.H>
#include <x/w/appearanceobj.H>
#include <x/w/tooltip_appearance.inc.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see tooltip_appearance

class tooltip_appearanceObj: public tooltip_appearance_properties,
			     public appearanceObj {

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
