/*
** Copyright 2017-2021 Double Precision, Inc.
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

class LIBCXX_HIDDEN defaultthemeObj;
class defaultthemeBase;

//! A loaded application theme.

typedef ref<defaultthemeObj, defaultthemeBase> defaulttheme;

//! A nullable pointer reference to a loaded application theme.

typedef ptr<defaultthemeObj, defaultthemeBase> defaultthemeptr;

//! A constant loaded application theme.

typedef const_ref<defaultthemeObj, defaultthemeBase> const_defaulttheme;

//! A constant nullable pointer reference to a loaded application theme.

typedef const_ptr<defaultthemeObj, defaultthemeBase> const_defaultthemeptr;

//! Current theme, mutex protected.

//! The current theme must be consulted when constructing new
//! widgets, and it is also accessed by the connection thread
//! as well.


typedef mpobj<const_defaulttheme, std::recursive_mutex> current_theme_t;

LIBCXXW_NAMESPACE_END

#endif
