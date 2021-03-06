/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_current_border_implfwd_h
#define x_w_impl_current_border_implfwd_h

#include <x/ptrfwd.H>
#include <x/w/namespace.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC current_border_implObj;

//! Current border around a widget.

//! Provides a \ref border_impl "border implementation object", and
//! a \c theme_updated() method that rebuilds it, after a theme change.

typedef ref<current_border_implObj> current_border_impl;

//! A nullable pointer reference to a \ref border "current border".

typedef ptr<current_border_implObj> current_border_implptr;

//! A constant \ref border "current border" object.

typedef const_ref<current_border_implObj> const_current_border_impl;

//! A nullable pointer reference to a constant \ref border "current border" object.

typedef const_ptr<current_border_implObj> const_current_border_implptr;

LIBCXXW_NAMESPACE_END

#endif
