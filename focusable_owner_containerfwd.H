/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef focusable_owner_containerfwd_h
#define focusable_owner_containerfwd_h

#include <x/ptrfwd.H>
#include "x/w/namespace.H"

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN focusable_owner_containerObj;

//! A focusable container, with ownership.

//! Multiply-inherits containerObj and focusableObj::ownerObj, implementing
//! a focusable container with ownership of the focusable object.

typedef ref<focusable_owner_containerObj> focusable_owner_container;

//! A nullable pointer reference to a \ref "focusable container owner".

typedef ptr<focusable_owner_containerObj> focusable_owner_containerptr;

//! A reference to a constant \ref "focusable container owner".

typedef const_ref<focusable_owner_containerObj> const_focusable_owner_container;

//! A nullable pointer reference to a \ref "focusable container owner".

typedef const_ptr<focusable_owner_containerObj> const_focusable_owner_containerptr;

LIBCXXW_NAMESPACE_END

#endif
