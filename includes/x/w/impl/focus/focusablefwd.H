/*
** Copyright 2017-2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_focusablefwd_h
#define x_w_impl_focusablefwd_h

#include <x/ref.H>
#include <x/w/namespace.H>
#include <list>

LIBCXXW_NAMESPACE_START

class focusableImplObj;

//! A focusable implementation mixin.

//! A display element that can received pointer or keyboard focus.
//!
//! There must be exactly one \ref focusableObj::ownerObj "owner object"
//! that officially owns a reference to the focusable object.
//!
//! A list of focusable fields is stored in the top level window handler
//! objects. This creates a circular reference, since display elements own
//! strong references to their parent display element, and the list becomes a
//! circular reference from the topmost display element to its child elements.
//!
//! The focusable owner object is responsible for creating the circular
//! reference in its constructor and removing it in the destructor.
//!
//! This focusable implementation object handles the mechanics of switching
//! keyboard input focus to the next/previous focusable object.

typedef ref<focusableImplObj> focusable_impl;

//! A nullable pointer reference to a \ref focusable_impl "focusable object".

typedef ptr<focusableImplObj> focusable_implptr;

//! A list of all focusable fields.

typedef std::list<focusable_impl> focusable_fields_t;

LIBCXXW_NAMESPACE_END

#endif
