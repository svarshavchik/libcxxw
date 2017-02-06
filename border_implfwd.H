/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef border_implfwd_h
#define border_implfwd_h

#include <x/ptrfwd.H>
#include "x/w/namespace.H"

LIBCXXW_NAMESPACE_START

class border_implObj;
class border_impl_base;
class border_info;

//! A border implementation

//! This object contains specifications for a border, and the logic to
//! draw it.

typedef ref<border_implObj, border_impl_base> border_impl;

//! A nullable pointer reference to a \ref border "border implementation".

typedef ptr<border_implObj, border_impl_base> border_implptr;

//! A constant \ref border "border implementation" object.

typedef const_ref<border_implObj, border_impl_base> const_border_impl;

//! A nullable pointer reference to a constant \ref border "border implementation" object.

typedef const_ptr<border_implObj, border_impl_base> const_border_implptr;

LIBCXXW_NAMESPACE_END

#endif
