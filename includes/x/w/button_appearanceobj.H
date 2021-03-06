/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_button_appearanceobj_h
#define x_w_button_appearanceobj_h

#include <x/w/button_appearancefwd.H>
#include <x/w/focus_border_appearancefwd.H>
#include <x/w/border_arg.H>
#include <x/w/font_arg.H>
#include <x/w/dim_arg.H>
#include <x/w/rgb.H>
#include <x/functional.H>
#include <x/w/appearanceobj.H>
#include <x/w/button_appearance.inc.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see button_appearance

class button_appearanceObj: public button_appearance_properties,
			    public appearanceObj {

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
