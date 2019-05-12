/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_bookpage_appearanceobj_h
#define x_w_bookpage_appearanceobj_h

#include <x/w/bookpage_appearancefwd.H>
#include <x/w/font_arg.H>
#include <x/w/dim_arg.H>
#include <x/w/rgb.H>
#include <x/functional.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Actual properties inherited by \ref bookpage_appearance "INSERT_LIBX_NAMESPACE::w::bookpage_appearance".

struct bookpage_appearance_properties {

	//! Constructor
	bookpage_appearance_properties();

	//! Destructor
	~bookpage_appearance_properties();

#include <x/w/bookpage_appearance.inc.H>

};

//! Implementation object.

//! \see bookpage_appearance

class bookpage_appearanceObj: public bookpage_appearance_properties,
				 virtual public obj {

public:
	//! Constructor
	bookpage_appearanceObj();

	//! Destructor
	~bookpage_appearanceObj();

	//! Copy constructor
	bookpage_appearanceObj(const bookpage_appearanceObj &);

	//! Create a copy of this object.
	template<typename F> inline const_bookpage_appearance modify(F &&f) const
	{
		return do_modify(make_function<void(const bookpage_appearance &)>
				 (std::forward<F>(f)));
	}

	//! Type-erased modify().
	const_bookpage_appearance do_modify(const function
					<void(const bookpage_appearance &)> &)
		const;
};

LIBCXXW_NAMESPACE_END

#endif
