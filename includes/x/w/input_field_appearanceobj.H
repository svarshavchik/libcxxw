/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_input_field_appearanceobj_h
#define x_w_input_field_appearanceobj_h

#include <x/w/input_field_appearancefwd.H>
#include <x/w/popup_list_appearancefwd.H>
#include <x/w/scrollbar_appearancefwd.H>
#include <x/w/focus_border_appearancefwd.H>
#include <x/w/tooltip_appearancefwd.H>
#include <x/w/border_arg.H>
#include <x/w/font_arg.H>
#include <x/w/dim_arg.H>
#include <x/w/rgb.H>
#include <x/w/button_appearancefwd.H>
#include <x/functional.H>
#include <x/w/appearanceobj.H>
#include <x/w/input_field_appearance.inc.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see input_field_appearance

class input_field_appearanceObj: public input_field_appearance_properties,
				 public appearanceObj {

public:
	//! Constructor
	input_field_appearanceObj();

	//! Destructor
	~input_field_appearanceObj();

	//! Copy constructor
	input_field_appearanceObj(const input_field_appearanceObj &);

	//! Create a copy of this object.
	template<typename F> inline const_input_field_appearance modify(F &&f)
		const
	{
		return do_modify(make_function
				 <void(const input_field_appearance &)>
				 (std::forward<F>(f)));
	}

	//! Type-erased modify().
	const_input_field_appearance
	do_modify(const function<void(const input_field_appearance &)> &) const;

	//! Update spinner button appearances.
	void update_spinner_buttons();
};

LIBCXXW_NAMESPACE_END

#endif
