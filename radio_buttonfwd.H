/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef radio_buttonfwd_h
#define radio_buttonfwd_h

#include "x/w/namespace.H"
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN radio_buttonObj;

/*! radio button widget implementation


*/

typedef ref<radio_buttonObj> radio_button;

//! A constant \ref radio_button "radio button widget implementation".

//! \see radio_button

typedef const_ref<radio_buttonObj> const_radio_button;

//! A nullable pointer reference to a \ref radio_button "radio button widget implementation".

//! \see radio_button

typedef ptr<radio_buttonObj> radio_buttonptr;

//! A nullable pointer reference to a const \ref radio_button "radio button widget implementation".

//! \see radio_button

typedef const_ptr<radio_buttonObj> const_radio_buttonptr;

LIBCXXW_NAMESPACE_END

#endif
