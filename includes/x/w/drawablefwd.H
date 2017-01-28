/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_drawablefwd_H
#define x_w_drawablefwd_H

#include <x/ptrfwd.H>
#include <x/w/namespace.H>

LIBCXXW_NAMESPACE_START

class drawableObj;

//! A drawable object.

//! A common superclass of \ref generic_window "generic windows" and
//! \ref pixmap "pixmaps".
//!
//! \code
//!
//! // Return the drawable's screen
//!
//! auto s=drawable->get_screen();
//!
//! \endcode
//!
//! \c get_screen() returns the drawable's \ref screen "screen".
//!
//! \code
//!
//! auto p=drawable->create_picture();
//!
//! \endcode
//!
//! Create a \ref picture "picture object" for this drawable.

typedef ref<drawableObj> drawable;

//! A nullable pointer reference to a \ref drawable "drawable" to the server

typedef ptr<drawableObj> drawableptr;

//! A constant \ref drawable "drawable" to the server
typedef const_ref<drawableObj> const_drawable;

//! A nullable pointer reference to a constant \ref drawable "drawable" to the server

typedef const_ptr<drawableObj> const_drawableptr;

LIBCXXW_NAMESPACE_END

#endif
