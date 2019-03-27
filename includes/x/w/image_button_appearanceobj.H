/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_image_button_appearanceobj_h
#define x_w_image_button_appearanceobj_h

#include <x/w/image_button_appearancefwd.H>
#include <x/w/border_arg.H>
#include <x/w/alignment.H>
#include <x/functional.H>
#include <x/obj.H>
#include <vector>
#include <string>

LIBCXXW_NAMESPACE_START

//! Actual properties inherited by \ref image_button_appearance "INSERT_LIBX_NAMESPACE::w::image_button_appearance".

struct image_button_appearance_properties {

	//! Constructor
	image_button_appearance_properties(const std::vector<std::string> &i);

	//! Destructor
	~image_button_appearance_properties();

	//! Vertical alignment, defaults to \c middle
	valign alignment;

	//! Default focus off border

	//! It is a transparent border the same size as the focus_on border.
	border_arg focusoff_border;

	//! Default focus on border
	border_arg focuson_border;

	//! Replace focusoff_border with a visible one.

	//! Default focusoff_border of the same size, but visually
	//! distinct.

	void set_distinct_focusoff_border();

	//! Checkbox images

	//! Specifies filenames the checkbox images get loaded from.
	//!
	//! \note
	//! Filenames without a path component get searched for in the
	//! current display theme's directory, and NOT the current
	//! directory.
	//!
	//! All images' sizes should be the same.

	std::vector<std::string> images;
};

//! Implementation object.

//! \see image_button_appearance

class image_button_appearanceObj: public image_button_appearance_properties,
				  virtual public obj {

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
