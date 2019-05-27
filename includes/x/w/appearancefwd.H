/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_appearancefwd_h
#define x_w_appearancefwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC appearanceObj;

/*! Appearance object.

An object that describes the visual appearance of properties of some
widget.

This is a base class that widget-specific appearance objects
derived from.

A \ref uigenerators "parsed theme file"'s get_appearance() method returns
a \c const_appearance object. The appearance objects are always \c const
objects, in order to be thread-safe because they can be referenced by the
connection thread at any time. The only time a regular \c appearance object
is exposed is when one gets modify()ed to create a new appearance object.

*/

typedef ref<appearanceObj> appearance;

//! A constant \ref appearance "appearance object".

//! \see appearance

typedef const_ref<appearanceObj> const_appearance;

//! A nullable pointer reference to an \ref appearance "appearance object".

//! \see appearance

typedef ptr<appearanceObj> appearanceptr;

//! A nullable pointer reference to a const \ref appearance "appearance object".

//! \see appearance

typedef const_ptr<appearanceObj> const_appearanceptr;

LIBCXXW_NAMESPACE_END

#endif
