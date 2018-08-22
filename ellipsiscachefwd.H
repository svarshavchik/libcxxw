/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_ellipsiscachefwd_h
#define x_w_ellipsiscachefwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class ellipsiscacheObj;

/*! Cache of ellipsis richtext objects shown by labels that are too big.

Caches richtext objects for drawing ellipsis in labels that are too
big to fit within their alloted space.

The richtext objects are cached by their fonts. This object is not
resposible for theme_updating() the fonts, or the richtexts.

*/

typedef ref<ellipsiscacheObj> ellipsiscache;

//! A constant \ref ellipsiscache "ellipsis cache".

//! \see ellipsiscache

typedef const_ref<ellipsiscacheObj> const_ellipsiscache;

//! A nullable pointer reference to an \ref ellipsiscache "ellipsis cache".

//! \see ellipsiscache

typedef ptr<ellipsiscacheObj> ellipsiscacheptr;

//! A nullable pointer reference to a const \ref ellipsiscache "ellipsis cache".

//! \see ellipsiscache

typedef const_ptr<ellipsiscacheObj> const_ellipsiscacheptr;

LIBCXXW_NAMESPACE_END

#endif
