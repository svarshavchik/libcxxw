/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_item_layout_appearanceobj_h
#define x_w_item_layout_appearanceobj_h

#include <x/w/item_layout_appearancefwd.H>
#include <x/w/scrollbar_appearancefwd.H>
#include <x/w/border_arg.H>
#include <x/w/dim_arg.H>
#include <x/w/rgb.H>
#include <x/functional.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Actual properties inherited by \ref item_layout_appearance "INSERT_LIBX_NAMESPACE::w::item_layout_appearance".

struct item_layout_appearance_properties {

	//! Constructor
	item_layout_appearance_properties();

	//! Destructor
	~item_layout_appearance_properties();

#include <x/w/item_layout_appearance.inc.H>

};

//! Implementation object.

//! \see item_layout_appearance

class item_layout_appearanceObj: public item_layout_appearance_properties,
				 virtual public obj {

public:
	//! Constructor
	item_layout_appearanceObj();

	//! Destructor
	~item_layout_appearanceObj();

	//! Copy constructor
	item_layout_appearanceObj(const item_layout_appearanceObj &);

	//! Create a copy of this object.

	template<typename F> inline const_item_layout_appearance modify(F &&f)
		const
	{
		return do_modify(make_function
				 <void(const item_layout_appearance &)>
				 (std::forward<F>(f)));
	}

	//! Type-erased modify().
	const_item_layout_appearance
	do_modify(const function<void(const item_layout_appearance &)> &) const;
};

LIBCXXW_NAMESPACE_END

#endif
