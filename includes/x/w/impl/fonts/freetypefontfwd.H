/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_impl_fonts_freetypefontfwd_H
#define x_w_impl_fonts_freetypefontfwd_H

#include <x/ptrfwd.H>
#include <x/w/namespace.H>

LIBCXXW_NAMESPACE_START

class freetypefontObj;

//! A loaded freetype font.

//! Loaded by a \ref fontcollection "fontcollection", one font suitable for
//! rendering selected characters.
//!
//! Implements load_glyphs(), that loads characters' bitmaps into an X
//! glyphset, after which they can be collected into a composite text stream
//! using glyphs_to_stream().
//!
//! glyphs_size_and_kernings() computes the size and the kerning of each
//! glyph.

//! \internal

typedef ref<freetypefontObj> freetypefont;

//! A nullable pointer reference to an \ref font "font"

typedef ptr<freetypefontObj> freetypefontptr;

//! A constant \ref font "font"
typedef const_ref<freetypefontObj> const_freetypefont;

//! A nullable pointer reference to a constant \ref font "font"

typedef const_ptr<freetypefontObj> const_freetypefontptr;

LIBCXXW_NAMESPACE_END

#endif
