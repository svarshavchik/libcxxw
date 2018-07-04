/*
** Copyright 2017-2018 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_impl_fonts_current_fontcollectionfwd_H
#define x_w_impl_fonts_current_fontcollectionfwd_H

#include <x/ptrfwd.H>
#include <x/w/namespace.H>

LIBCXXW_NAMESPACE_START

class current_fontcollectionObj;

//! A font collection based on the current theme.

//! A \ref current_fontcollection "font collection" that's accessible only to
//! the current execution thread.
//!
//! theme_updated() recreates the font collection after a new theme is
//! installed.
//!
//! This is an internal object that's visible through the public library API.
//! A \ref font "font" descriptor gets converted to this handle, internally,
//! with the handle still visible as part of other temporary classes that are
//! used in the API, hence this handle must be visible in the API.

typedef ref<current_fontcollectionObj> current_fontcollection;

//! A nullable pointer reference to \ref current_fontcollection "current font collection".

typedef ptr<current_fontcollectionObj> current_fontcollectionptr;

//! Constant \ref current_fontcollection "current font collection".

typedef const_ref<current_fontcollectionObj> const_current_fontcollection;

//! A nullable pointer reference to constant \ref current_fontcollection "current font collection".

typedef const_ptr<current_fontcollectionObj> const_current_fontcollectionptr;

LIBCXXW_NAMESPACE_END

#endif
