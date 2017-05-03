/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef icon_cache_H
#define icon_cache_H

#include <x/ref.H>
#include "icon_cachefwd.H"
#include "icon_cacheobj.H"
#include "sxg/sxg_parser.H"
#include "sxg/sxg_image.H"
#include <x/weakunordered_multimap.H>

#include <experimental/string_view>

LIBCXXW_NAMESPACE_START

//! Load and cache an SXG file

//! Return an SXG parser object for the given filename, screen, and theme.
//! The SXG parser object depends on the screen and theme.
//!
//! If filename does not have a path, and it doesn't exist in the current
//! directory, the SXG file gets loaded from the theme directory.
//!
//! The same theme filename, screen, and theme, returns the same SXG object.
//! It is cached.

sxg_parser get_sxg(const std::experimental::string_view &filename,
		   const screen &screenref,
		   const defaulttheme &theme) LIBCXX_HIDDEN;

LIBCXXW_NAMESPACE_END

#endif
