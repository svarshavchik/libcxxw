/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_progressbar_appearanceobj_h
#define x_w_progressbar_appearanceobj_h

#include <x/w/progressbar_appearancefwd.H>
#include <x/w/border_arg.H>
#include <x/w/font_arg.H>
#include <x/w/dim_arg.H>
#include <x/w/rgb.H>
#include <x/functional.H>
#include <x/w/appearanceobj.H>

LIBCXXW_NAMESPACE_START

//! Actual properties inherited by \ref progressbar_appearance "INSERT_LIBX_NAMESPACE::w::progressbar_appearance".

struct progressbar_appearance_properties {

	//! Constructor
	progressbar_appearance_properties();

	//! Destructor
	~progressbar_appearance_properties();

#include <x/w/progressbar_appearance.inc.H>

};

//! Implementation object.

//! \see progressbar_appearance

class progressbar_appearanceObj: public progressbar_appearance_properties,
				 public appearanceObj {

public:
	//! Constructor
	progressbar_appearanceObj();

	//! Destructor
	~progressbar_appearanceObj();

	//! Copy constructor
	progressbar_appearanceObj(const progressbar_appearanceObj &);

	//! Create a copy of this object.
	template<typename F> inline const_progressbar_appearance modify(F &&f) const
	{
		return do_modify(make_function<void(const progressbar_appearance &)>
				 (std::forward<F>(f)));
	}

	//! Type-erased modify().
	const_progressbar_appearance do_modify(const function
					<void(const progressbar_appearance &)> &)
		const;
};

LIBCXXW_NAMESPACE_END

#endif
