/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_fontcollectionfwd_H
#define x_w_fontcollectionfwd_H

#include <x/ptrfwd.H>
#include <x/w/namespace.H>

LIBCXXW_NAMESPACE_START

class fontcollectionObj;

//! A collection of fonts.

//! This object holds a collection of
//! \ref freetypefont "fonts" for rendering text. A single font specification
//! may require loading multiple fonts in order to render every character in
//! the rendered text. The fontconfig library prepares a list of fonts, to
//! be searched in order for rendering specific characters.
//!
//! The actual list of fonts is stored in the
//! \ref fontconfig::sortedlist "fontconfig::sortedlist", this object
//! implements the logic for searching the font list.
//!
//! The methods in this class take a sequence of unicode characters specified
//! by a beginning and an ending iterator, then processes them as follows:
//!
//! load_glyphs() looks up the fonts for the specified characters, then invokes
//! each \ref freetypefont "freetypefont"'s load_glyphs() method.
//!
//! glyphs_size_and_kernings() computes the size and the kerning of each
//! character.
//!
//! lookup() is a generic function that invokes a callback, passing a
//! beginning and an ending iterator to the callback together with a
//! \ref freetypefont "freetypefont" for rendering the text. If the entire
//! text sequence is renderable by one font, the callback gets invoked
//! only one time; otherwise the callback gets repeatedly invoked for each
//! portion of the text range that's renderable by its individual font, from
//! the font collection.
//!
//! The fontcollection object also exposes some composite metrics: ascender,
//! descender, etc... These metrics are derived only from the currently loaded
//! fonts, so they may actually change, if the fontcollection object must
//! load additional fonts in order to render characters.

typedef ref<fontcollectionObj> fontcollection;

//! A nullable pointer reference to \ref fontcollection "font collection".

typedef ptr<fontcollectionObj> fontcollectionptr;

//! Constant \ref fontcollection "font collection".

typedef const_ref<fontcollectionObj> const_fontcollection;

//! A nullable pointer reference to constant \ref fontcollection "font collection".

typedef const_ptr<fontcollectionObj> const_fontcollectionptr;

LIBCXXW_NAMESPACE_END

#endif
