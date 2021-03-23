/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_focus_border_appearanceobj_h
#define x_w_focus_border_appearanceobj_h

#include <x/w/focus_border_appearancefwd.H>
#include <x/w/border_arg.H>
#include <x/w/font_arg.H>
#include <x/w/dim_arg.H>
#include <x/w/rgb.H>
#include <x/functional.H>
#include <x/w/appearanceobj.H>
#include <x/w/focus_border_appearance.inc.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see focus_border_appearance

class focus_border_appearanceObj: public focus_border_appearance_properties,
				  public appearanceObj {

public:
	//! Constructor
	focus_border_appearanceObj();

	//! Destructor
	~focus_border_appearanceObj();

	//! Copy constructor
	focus_border_appearanceObj(const focus_border_appearanceObj &);

	//! Create a copy of this object.
	template<typename F> inline const_focus_border_appearance modify(F &&f) const
	{
		return do_modify(make_function<void(const focus_border_appearance &)>
				 (std::forward<F>(f)));
	}

	//! Type-erased modify().
	const_focus_border_appearance do_modify(const function
					<void(const focus_border_appearance &)> &)
		const;
};

LIBCXXW_NAMESPACE_END

#endif
