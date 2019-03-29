/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_frame_appearanceobj_h
#define x_w_frame_appearanceobj_h

#include <x/w/frame_appearancefwd.H>
#include <x/w/border_arg.H>
#include <x/w/font_arg.H>
#include <x/w/dim_arg.H>
#include <x/w/rgb.H>
#include <x/functional.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Actual properties inherited by \ref frame_appearance "INSERT_LIBX_NAMESPACE::w::frame_appearance".

struct frame_appearance_properties {

	//! Constructor
	frame_appearance_properties();

	//! Destructor
	~frame_appearance_properties();

	//! The border around the frame.

	border_arg border;

	//! Title indentation
	dim_arg title_indent;

	//! Disable automatic background.

	//! Frame background color

	//! If no_background is not set, the frame's background.
	color_arg frame_background;

	//! Extra padding between the border and the element in the container

	dim_arg hpad;

	//! Extra padding between the border and the element in the container
	dim_arg vpad;
};

//! Implementation object.

//! \see frame_appearance

class frame_appearanceObj: public frame_appearance_properties,
				 virtual public obj {

public:
	//! Constructor
	frame_appearanceObj();

	//! Destructor
	~frame_appearanceObj();

	//! Copy constructor
	frame_appearanceObj(const frame_appearanceObj &);

	//! Create a copy of this object.
	template<typename F> inline const_frame_appearance modify(F &&f) const
	{
		return do_modify(make_function<void(const frame_appearance &)>
				 (std::forward<F>(f)));
	}

	//! Type-erased modify().
	const_frame_appearance do_modify(const function
					<void(const frame_appearance &)> &)
		const;
};

LIBCXXW_NAMESPACE_END

#endif
