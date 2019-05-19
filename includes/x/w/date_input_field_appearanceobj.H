/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_date_input_field_appearanceobj_h
#define x_w_date_input_field_appearanceobj_h

#include <x/w/date_input_field_appearancefwd.H>
#include <x/w/input_field_appearancefwd.H>
#include <x/w/image_button_appearancefwd.H>
#include <x/w/generic_window_appearancefwd.H>
#include <x/w/border_arg.H>
#include <x/w/font_arg.H>
#include <x/w/dim_arg.H>
#include <x/w/rgb.H>
#include <x/functional.H>
#include <x/w/appearanceobj.H>

LIBCXXW_NAMESPACE_START

//! Actual properties inherited by \ref date_input_field_appearance "INSERT_LIBX_NAMESPACE::w::date_input_field_appearance".

struct date_input_field_appearance_properties {

	//! Constructor
	date_input_field_appearance_properties();

	//! Destructor
	~date_input_field_appearance_properties();

#include <x/w/date_input_field_appearance.inc.H>

};

//! Implementation object.

//! \see date_input_field_appearance

class date_input_field_appearanceObj
	: public date_input_field_appearance_properties,
	  public appearanceObj {

public:
	//! Constructor
	date_input_field_appearanceObj();

	//! Destructor
	~date_input_field_appearanceObj();

	//! Copy constructor
	date_input_field_appearanceObj(const date_input_field_appearanceObj &);

	//! Create a copy of this object.

	template<typename F>
	inline const_date_input_field_appearance modify(F &&f) const
	{
		return do_modify(make_function<void(const date_input_field_appearance &)>
				 (std::forward<F>(f)));
	}

	//! Type-erased modify().
	const_date_input_field_appearance
	do_modify(const function<void(const date_input_field_appearance &)> &)
		const;
};

LIBCXXW_NAMESPACE_END

#endif
