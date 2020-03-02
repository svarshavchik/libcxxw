/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_iconfwd_h
#define x_w_impl_iconfwd_h

#include <x/ptrfwd.H>
#include <x/w/namespace.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN iconObj;

//! A loaded icon.

//! Holds a reference to an icon.
//!
//! After construction, at some point initialize() gets invoked from the
//! connection thread, to finish initialization.
//!
//! theme_updated() gets invoked, in some fashion, to notify the icon that
//! the display theme has changed, this may result in a replacement image.
//!
//! Both initialize() and theme_updated() return an icon object that should
//! be used going forward. It's possible that between the time the icon
//! gets constructed, and initialize(), the current theme has changed, in
//! which case initialize() returns the new icon for the new theme; otherwise
//! initialize() returns the same object. Similarly, theme_updated() usually
//! brings a new icon from the new theme.

typedef ref<iconObj> icon;

//! A nullable pointer reference to an \ref icon "icon".

typedef ptr<iconObj> iconptr;

//! A reference to a constant \ref icon "icon".

typedef const_ref<iconObj> const_icon;

//! A nullable pointer reference to a constant \ref icon "icon".

typedef const_ptr<iconObj> const_iconptr;

LIBCXXW_NAMESPACE_END

#endif
