/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_bookpage_appearanceobj_h
#define x_w_bookpage_appearanceobj_h

#include <x/w/bookpage_appearancefwd.H>
#include <x/w/font_arg.H>
#include <x/w/dim_arg.H>
#include <x/w/rgb.H>
#include <x/functional.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Actual properties inherited by \ref bookpage_appearance "INSERT_LIBX_NAMESPACE::w::bookpage_appearance".

struct bookpage_appearance_properties {

	//! Constructor
	bookpage_appearance_properties();

	//! Destructor
	~bookpage_appearance_properties();

	//! Inactive tab color
	color_arg noncurrent_color;

	//! Active tab color
	color_arg current_color;

	//! Color of the tab when it has keyboard or pointer focus
	color_arg warm_color;

	//! Color of the tab when it's been activated.
	color_arg active_color;

	//! Default font for the tab
	font_arg label_font;

	//! Default font's color
	color_arg label_foreground_color;

	//! Tab padding.
	dim_arg horiz_padding;

	//! Padding for an individual tab.
	dim_arg vert_padding;
};

//! Implementation object.

//! \see bookpage_appearance

class bookpage_appearanceObj: public bookpage_appearance_properties,
				 virtual public obj {

public:
	//! Constructor
	bookpage_appearanceObj();

	//! Destructor
	~bookpage_appearanceObj();

	//! Copy constructor
	bookpage_appearanceObj(const bookpage_appearanceObj &);

	//! Create a copy of this object.
	template<typename F> inline const_bookpage_appearance modify(F &&f) const
	{
		return do_modify(make_function<void(const bookpage_appearance &)>
				 (std::forward<F>(f)));
	}

	//! Type-erased modify().
	const_bookpage_appearance do_modify(const function
					<void(const bookpage_appearance &)> &)
		const;
};

LIBCXXW_NAMESPACE_END

#endif
