/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_scrollbar_appearanceobj_h
#define x_w_scrollbar_appearanceobj_h

#include <x/w/scrollbar_appearancefwd.H>
#include <x/w/border_arg.H>
#include <x/w/rgb.H>
#include <x/functional.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Actual properties inherited by \ref scrollbar_appearance "INSERT_LIBX_NAMESPACE::w::scrollbar_appearance".

struct scrollbar_appearance_properties {

	//! Constructor
	scrollbar_appearance_properties();

	//! Destructor
	~scrollbar_appearance_properties();

	//! Scroll-bar's background color
	color_arg background_color;

	//! No focus around the scrollbar
	border_arg focusoff_border;

	//! Focus around the scrollbar
	border_arg focuson_border;
};

//! Implementation object.

//! \see scrollbar_appearance

class scrollbar_appearanceObj: public scrollbar_appearance_properties,
				 virtual public obj {

public:
	//! Constructor
	scrollbar_appearanceObj();

	//! Destructor
	~scrollbar_appearanceObj();

	//! Copy constructor
	scrollbar_appearanceObj(const scrollbar_appearanceObj &);

	//! Create a copy of this object.
	template<typename F> inline const_scrollbar_appearance modify(F &&f)
		const
	{
		return do_modify(make_function
				 <void(const scrollbar_appearance &)>
				 (std::forward<F>(f)));
	}

	//! Type-erased modify().
	const_scrollbar_appearance
	do_modify(const function<void(const scrollbar_appearance &)> &) const;
};

LIBCXXW_NAMESPACE_END

#endif
