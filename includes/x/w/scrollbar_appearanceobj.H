/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_scrollbar_appearanceobj_h
#define x_w_scrollbar_appearanceobj_h

#include <x/w/scrollbar_appearancefwd.H>
#include <x/w/scrollbar_images_appearancefwd.H>
#include <x/w/focus_border_appearancefwd.H>
#include <x/w/border_arg.H>
#include <x/w/rgb.H>
#include <x/functional.H>
#include <x/w/appearanceobj.H>
#include <x/w/scrollbar_appearance.inc.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see scrollbar_appearance

class scrollbar_appearanceObj: public scrollbar_appearance_properties,
			       public appearanceObj {

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
