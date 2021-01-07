/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_list_appearanceobj_h
#define x_w_list_appearanceobj_h

#include <x/w/list_appearancefwd.H>
#include <x/w/generic_window_appearancefwd.H>
#include <x/w/border_arg.H>
#include <x/w/font_arg.H>
#include <x/w/rgb.H>
#include <x/w/dim_arg.H>
#include <x/w/scrollbar_appearancefwd.H>
#include <x/w/focus_border_appearancefwd.H>
#include <x/functional.H>
#include <x/w/appearanceobj.H>

LIBCXXW_NAMESPACE_START

//! All properties inherited by a \ref list_appearance "list appearance object".

struct list_appearance_properties {

	//! Constructor
	list_appearance_properties();

	//! Destructor
	~list_appearance_properties();

#include <x/w/list_appearance.inc.H>

};

//! Implementation object.

//! \see list_appearance

class list_appearanceObj: public list_appearance_properties,
			  public appearanceObj {

public:
	//! Constructor
	list_appearanceObj();

	//! Destructor
	~list_appearanceObj();

	//! Copy constructor
	list_appearanceObj(const list_appearanceObj &);

	//! Clone this object.

	template<typename F> inline const_list_appearance modify(F &&f) const
	{
		return do_modify(make_function<void(const list_appearance &)>
				 (std::forward<F>(f)));
	}

	//! Type-erased modify().
	const_list_appearance
	do_modify(const function<void(const list_appearance &)> &) const;
};

LIBCXXW_NAMESPACE_END

#endif
