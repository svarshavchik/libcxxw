/*
** Copyright 2017-2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_background_colorobj_H
#define x_w_impl_background_colorobj_H

#include <x/obj.H>
#include <x/w/picturefwd.H>
#include <x/w/elementobj.H>
#include <x/w/connection_threadfwd.H>
#include <x/w/impl/defaultthemefwd.H>

LIBCXXW_NAMESPACE_START

//! An abstract handle for a "background colors".

//! A background color is just a picture. This abstracts away the picture
//! object, and implements initialize() and theme_updated().

class background_colorObj : virtual public obj {

public:
	//! Constructor
	background_colorObj();

	//! Destructor
	~background_colorObj();

	//! Adjust this background color for an element of the given size.

	//! If the background color is a gradient, this gets adjusted
	//! accordingly.

	virtual background_color get_background_color_for(ONLY IN_THREAD,
							  elementObj::implObj &)
		=0;

	//! Whether this background color is scrollable, by a peephole.

	//! Basically returns true if the background color is a solid color
	//! picture, and false for all other kinds of backgrounds.

	virtual bool is_scrollable_background()=0;

	//! Return the current color picture.
	virtual const_picture get_current_color(ONLY IN_THREAD)=0;

	//! This background color's display element is about to be initialized.
	virtual void initialize(ONLY IN_THREAD)=0;

	//! The current display theme has been changed.
	virtual void theme_updated(ONLY IN_THREAD,
				   const defaulttheme &new_theme)=0;
};

LIBCXXW_NAMESPACE_END
#endif