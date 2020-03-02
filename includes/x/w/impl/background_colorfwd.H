/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_background_colorfwd_H
#define x_w_impl_background_colorfwd_H

#include <x/ptrfwd.H>
#include <x/w/namespace.H>

LIBCXXW_NAMESPACE_START

class background_colorObj;

//! A background color, possibly dependent on the current theme.

//! This object produces, on demand, a "background color".
//!
//! get_current_color() returns the picture for the background color.
//! theme_updated() gets invoked when the screen's theme changes. If this
//! is a theme-dependent background color, all subsequent calls to
//! get_current_color() will return a new picture.
//!
//! This is a slight misnomer. The resulting picture can be used for any
//! purpose, not just drawing the background color. Foreground colors,
//! sideways colors, etc...
//!
//! The background color objects are automcally cached. Background color objects
//! individually created for the same underlying color will end up being
//! the same object. recycled_pixmaps contains caches for both theme-
//! dependent and theme-independent background colors.

typedef ref<background_colorObj> background_color;

//! A nullable pointer reference to a \ref background_color "background color".

typedef ptr<background_colorObj> background_colorptr;

LIBCXXW_NAMESPACE_END
#endif
