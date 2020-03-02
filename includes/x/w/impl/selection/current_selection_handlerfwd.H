/*
** Copyright 2018-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_current_selection_handlerfwd_h
#define x_w_impl_current_selection_handlerfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN current_selection_handlerObj;

/*! Base class for received X selection.

A conversion_handler that gets installed if convert_selection() gives the
green light.

Handles the converted data. generic_windowObj::handlerObj implements the
conversion callbacks from window_handlerObj, and invokes the corresponding
methods in the base class, passing in *this, and letting a subclass implement
the entire conversion process.

\see window_handlerObj

*/

typedef ref<current_selection_handlerObj> current_selection_handler;

//! A constant \ref current_selection_handler "X selection handler".

//! \see current_selection_handler

typedef const_ref<current_selection_handlerObj> const_current_selection_handler;

//! A nullable pointer reference to an \ref current_selection_handler "X selection handler".

//! \see current_selection_handler

typedef ptr<current_selection_handlerObj> current_selection_handlerptr;

//! A nullable pointer reference to a const \ref current_selection_handler "X selection handler".

//! \see current_selection_handler

typedef const_ptr<current_selection_handlerObj> const_current_selection_handlerptr;

LIBCXXW_NAMESPACE_END

#endif
