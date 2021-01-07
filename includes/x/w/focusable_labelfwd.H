/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_focusable_labelfwd_h
#define x_w_focusable_labelfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC focusable_labelObj;

struct focusable_label_config;

/*! A label that processes input focus.

A subclass of a \ref label "label" that has a focus frame and offers
callbacks for processing keyboard input.

It also inherits from a container object, this is for internal use only.

This widget implements the shown current value in combo-boxes.

*/

typedef ref<focusable_labelObj> focusable_label;

//! A constant \ref focusable_label "focusable label".

//! \see focusable_label

typedef const_ref<focusable_labelObj> const_focusable_label;

//! A nullable pointer reference to a \ref focusable_label "focusable label".

//! \see focusable_label

typedef ptr<focusable_labelObj> focusable_labelptr;

//! A nullable pointer reference to a const \ref focusable_label "focusable label".

//! \see focusable_label

typedef const_ptr<focusable_labelObj> const_focusable_labelptr;

LIBCXXW_NAMESPACE_END

#endif
