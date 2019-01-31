/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_size_hintsfwd_h
#define x_w_size_hintsfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN size_hintsObj;

/*! Helper object for setting size hints increment.

Helper object that's used to set width and height increments for
WM_SIZE_HINTS.

Something that prefers to be resized in increments passes this object
to install_size_hints(). It's expected that this object's destructor
invokes size_hints_updated(). This object is kept in a weak pointer,
so invoking uninstall_size_hints() from a destructor guarantees that
the weak pointer will now be null.

It goes without saying that a given window can only have one size hint
increment. An exception gets thrown otherwise.

*/

typedef ref<size_hintsObj> size_hints;

//! A constant \ref size_hints "size hints increment helper object".

//! \see size_hints

typedef const_ref<size_hintsObj> const_size_hints;

//! A nullable pointer reference to a \ref size_hints "size hints increment helper object".

//! \see size_hints

typedef ptr<size_hintsObj> size_hintsptr;

//! A nullable pointer reference to a const \ref size_hints "size hints increment helper object".

//! \see size_hints

typedef const_ptr<size_hintsObj> const_size_hintsptr;

LIBCXXW_NAMESPACE_END

#endif
