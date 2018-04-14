/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_focusable_container_ownerfwd_h
#define x_w_focusable_container_ownerfwd_h

#include <x/w/containerfwd.H>

LIBCXXW_NAMESPACE_START

class focusable_container_ownerObj;

//! A focusable container that's the registered owner of its focusable.

//! Multiply inherits from \ref focusable_container "focusable_container"
//! and a \ref focusableObj::ownerObj "focusable implementation owner".

typedef ref<focusable_container_ownerObj> focusable_container_owner;

//! A constant focusable container owner.

//! \see focusable_container_owner

typedef const_ref<focusable_container_ownerObj> const_focusable_container_owner;

//! A nullable pointer reference to a focusable container owner.

//! \see focusable_container_owner

typedef ptr<focusable_container_ownerObj> focusable_container_ownerptr;

//! A nullable pointer reference to a constant focusable container owner.

//! \see focusable_container_owner

typedef const_ptr<focusable_container_ownerObj
		  > const_focusable_container_ownerptr;

LIBCXXW_NAMESPACE_END

#endif
