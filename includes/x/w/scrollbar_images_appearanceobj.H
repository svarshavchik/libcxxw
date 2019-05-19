/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_scrollbar_images_appearanceobj_h
#define x_w_scrollbar_images_appearanceobj_h

#include <x/w/scrollbar_images_appearancefwd.H>
#include <x/w/border_arg.H>
#include <x/w/font_arg.H>
#include <x/w/dim_arg.H>
#include <x/w/rgb.H>
#include <x/functional.H>
#include <x/w/appearanceobj.H>

LIBCXXW_NAMESPACE_START

//! Images used in a \ref scrollbar_appearance "scrollbar appearance".

struct scrollbar_images_appearance_properties {

	//! Constructor
	scrollbar_images_appearance_properties();

	//! Destructor
	~scrollbar_images_appearance_properties();

#include <x/w/scrollbar_images_appearance.inc.H>

};

//! Implementation object.

//! \see scrollbar_images_appearance

class scrollbar_images_appearanceObj:
	public scrollbar_images_appearance_properties,
	public appearanceObj {

public:
	//! Constructor
	scrollbar_images_appearanceObj();

	//! Destructor
	~scrollbar_images_appearanceObj();

	//! Copy constructor
	scrollbar_images_appearanceObj(const scrollbar_images_appearanceObj &);

	//! Create a copy of this object.
	template<typename F> inline const_scrollbar_images_appearance modify(F &&f) const
	{
		return do_modify(make_function<void(const scrollbar_images_appearance &)>
				 (std::forward<F>(f)));
	}

	//! Type-erased modify().
	const_scrollbar_images_appearance do_modify(const function
					<void(const scrollbar_images_appearance &)> &)
		const;
};

LIBCXXW_NAMESPACE_END

#endif
