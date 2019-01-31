/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef freetypefwd_H
#define freetypefwd_H

#include <x/ptrfwd.H>
#include "x/w/namespace.H"

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN freetypeObj;
class freetypeBase;

class font_search_properties;

//! An internal object that encapsulates access to the Freetype library.

//! \c INSERT_LIBX_NAMESPACE::w::freetype is a ref handle for an internal
//! object that opens individual fonts. This class is typically not used
//! directly. It's used by other, higher-level methods, like a drawable's
//! font_search() method.

typedef ref<freetypeObj, freetypeBase> freetype;

//! A nullable pointer reference to an \ref freetype "freetype 2 library"

typedef ptr<freetypeObj, freetypeBase> freetypeptr;

//! A constant \ref freetype "freetype 2 library"
typedef const_ref<freetypeObj, freetypeBase> const_freetype;

//! A nullable pointer reference to a constant \ref freetype "freetype 2 library"

typedef const_ptr<freetypeObj, freetypeBase> const_freetypeptr;

LIBCXXW_NAMESPACE_END

#endif
