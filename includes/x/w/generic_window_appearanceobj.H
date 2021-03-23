/*
** Copyright 2019-2021 Double Precision, Inc.
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
#include <x/w/appearanceobj.H>
#include <vector>
#include <string>
#include <x/w/generic_window_appearance.inc.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see generic_window_appearance

class generic_window_appearanceObj: public generic_window_appearance_properties,
				    public appearanceObj {

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
