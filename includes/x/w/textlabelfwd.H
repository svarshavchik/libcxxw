/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_textlabelfwd_h
#define x_w_textlabelfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC textlabelObj;

//! Text label interface

//! Specifies the interface for a widget that contains a text label.
//!
//! \see label

typedef ref<textlabelObj> textlabel;

//! A nullable pointer reference to an text label.

//! \see textlabel

typedef ptr<textlabelObj> textlabelptr;

//! A reference to a constant text label object.

//! \see textlabel

typedef const_ref<textlabelObj> const_textlabel;

//! A nullable pointer reference to a constant text label object.

//! \see textlabel

typedef const_ptr<textlabelObj> const_textlabelptr;

LIBCXXW_NAMESPACE_END

#endif
