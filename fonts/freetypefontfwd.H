/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef freetypefontfwd_H
#define freetypefontfwd_H

#include <x/ptrfwd.H>
#include "x/w/namespace.H"

LIBCXXW_NAMESPACE_START

class freetypefontObj;
class freetypefontBase;

//! A loaded freetype font.

//! Loaded by a \ref fontcollection "fontcollection", one font suitable for
//! rendering selected characters.
//!
//! Implements load_glyphs(), that loads characters' bitmaps into an X
//! glyphset, after which they can be collected into a composite text stream
//! using glyphs_to_stream().
//!
//! The width of all the glyphs also gets computed by glyphs_width().

//! \internal

typedef ref<freetypefontObj, freetypefontBase> freetypefont;

//! A nullable pointer reference to an \ref font "font"

typedef ptr<freetypefontObj, freetypefontBase> freetypefontptr;

//! A constant \ref font "font"
typedef const_ref<freetypefontObj, freetypefontBase> const_freetypefont;

//! A nullable pointer reference to a constant \ref font "font"

typedef const_ptr<freetypefontObj, freetypefontBase> const_freetypefontptr;

LIBCXXW_NAMESPACE_END

#endif
