/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef background_color_H
#define background_color_H

#include <x/ref.H>
#include "background_colorfwd.H"
#include "background_colorobj.H"

LIBCXXW_NAMESPACE_START

//! Base class for \ref background_color "background colors".

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::background_color::base.

class background_colorBase : public ptrrefBase {
public:

	//! Create a background color object that's not tied to a theme color.

	static background_color
	create_background_color(const const_picture &color);
};

LIBCXXW_NAMESPACE_END

#endif
