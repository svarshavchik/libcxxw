/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_labelfwd_h
#define x_w_labelfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class labelObj;

//! A text label display element.

//! Multiply-inherits from \ref textlabel "textlabel" and
//! \ref element "element" for a normal label display element.
//!
//! \code
//! factory->create_label({"Hello world!"});
//! \endcode
//!
//! \see factory

typedef ref<labelObj> label;

//! A nullable pointer reference to an label.

//! \see label

typedef ptr<labelObj> labelptr;

//! A reference to a constant label object.

//! \see label

typedef const_ref<labelObj> const_label;

//! A nullable pointer reference to a constant label object.

//! \see label

typedef const_ptr<labelObj> const_labelptr;

LIBCXXW_NAMESPACE_END

#endif
