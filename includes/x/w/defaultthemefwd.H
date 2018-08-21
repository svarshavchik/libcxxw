/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_defaultthemefwd_h
#define x_w_defaultthemefwd_h

#include <x/ptrfwd.H>
#include <x/mpobj.H>
#include <x/w/dim_arg.H>
#include <x/w/types.H>
#include <variant>

LIBCXXW_NAMESPACE_START

//! Location of the directory with all the themes

std::string themedirroot();

class defaultthemeObj;
class defaultthemeBase;

class rgb;
struct linear_gradient;
struct radial_gradient;

//! A theme color is either an rgb solid color, or a gradient color.

typedef std::variant<rgb, linear_gradient, radial_gradient> theme_color_t;

//! A loaded application theme.

typedef ref<defaultthemeObj, defaultthemeBase> defaulttheme;

//! A nullable pointer reference to a loaded application theme.

typedef ptr<defaultthemeObj, defaultthemeBase> defaultthemeptr;

//! Current theme, mutex protected.

//! The current theme must be consulted when constructing new
//! display elements, and it is also accessed by the connection thread
//! as well.


typedef mpobj<defaulttheme, std::recursive_mutex> current_theme_t;

LIBCXXW_NAMESPACE_END

#endif
