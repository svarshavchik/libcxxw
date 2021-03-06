/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_item_button_appearanceobj_h
#define x_w_item_button_appearanceobj_h

#include <x/w/item_button_appearancefwd.H>
#include <x/w/image_button_appearancefwd.H>
#include <x/w/border_arg.H>
#include <x/w/font_arg.H>
#include <x/w/dim_arg.H>
#include <x/w/rgb.H>
#include <x/functional.H>
#include <x/w/appearanceobj.H>
#include <x/w/item_button_appearance.inc.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see item_button_appearance

class item_button_appearanceObj: public item_button_appearance_properties,
				 public appearanceObj {

public:
	//! Constructor
	item_button_appearanceObj();

	//! Destructor
	~item_button_appearanceObj();

	//! Copy constructor
	item_button_appearanceObj(const item_button_appearanceObj &);

	//! Create a copy of this object.

	template<typename F> inline const_item_button_appearance modify(F &&f)
		const
	{
		return do_modify(make_function
				 <void(const item_button_appearance &)>
				 (std::forward<F>(f)));
	}

	//! Type-erased modify().
	const_item_button_appearance
	do_modify(const function<void(const item_button_appearance &)> &) const;
};

LIBCXXW_NAMESPACE_END

#endif
