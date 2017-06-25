/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef defaultthemefwd_h
#define defaultthemefwd_h

#include <x/ptrfwd.H>
#include <x/mpobj.H>
#include "x/w/types.H"

#include <experimental/string_view>

LIBCXXW_NAMESPACE_START

//! Location of the directory with all the themes

std::string themedirroot();

class defaultthemeObj;
class defaultthemeBase;

//! A loaded application theme.

typedef ref<defaultthemeObj, defaultthemeBase> defaulttheme;

//! A nullable pointer reference to a loaded application theme.

typedef ptr<defaultthemeObj, defaultthemeBase> defaultthemeptr;

//! Current theme, mutex protected.

//! The current theme must be consulted when constructing new
//! display elements, and it is also accessed by the connection thread
//! as well.


typedef mpobj<defaulttheme> current_theme_t;

//! Retrieve a width or a height theme dimension.

typedef dim_t (defaultthemeObj::*theme_width_or_height)
	(const std::experimental::string_view &);

LIBCXXW_NAMESPACE_END

#endif
