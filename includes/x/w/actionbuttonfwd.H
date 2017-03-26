/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_actionbuttonfwd_h
#define x_w_actionbuttonfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class actionbuttonObj;

//! An action button.


typedef ref<actionbuttonObj> actionbutton;

//! A nullable pointer reference to an action button.

typedef ptr<actionbuttonObj> actionbuttonptr;

//! A reference to a constant action button.

//! \see actionbutton

typedef const_ref<actionbuttonObj> const_actionbutton;

//! A nullable pointer reference to a constant action button.

//! \see actionbutton

typedef const_ptr<actionbuttonObj> const_actionbuttonptr;

LIBCXXW_NAMESPACE_END

#endif
