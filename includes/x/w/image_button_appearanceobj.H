/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_image_button_appearanceobj_h
#define x_w_image_button_appearanceobj_h

#include <x/w/image_button_appearancefwd.H>
#include <x/w/focus_border_appearancefwd.H>
#include <x/w/border_arg.H>
#include <x/w/alignment.H>
#include <x/functional.H>
#include <x/w/appearanceobj.H>
#include <vector>
#include <string>
#include <x/w/image_button_appearance.inc.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see image_button_appearance

class image_button_appearanceObj: public image_button_appearance_properties,
				  public appearanceObj {

public:
	//! Constructor
	image_button_appearanceObj(const std::vector<std::string> &images);

	//! Destructor
	~image_button_appearanceObj();

	//! Copy constructor
	image_button_appearanceObj(const image_button_appearanceObj &);

	//! Create a copy of this object.
	template<typename F> inline const_image_button_appearance modify(F &&f)
		const
	{
		return do_modify(make_function
				 <void(const image_button_appearance &)>
				 (std::forward<F>(f)));
	}

	//! Type-erased modify().
	const_image_button_appearance
	do_modify(const function<void(const image_button_appearance &)> &)
		const;
};

LIBCXXW_NAMESPACE_END

#endif
