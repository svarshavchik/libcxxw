/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_input_fieldfwd_h
#define x_w_input_fieldfwd_h

#include <x/w/input_field_configfwd.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class input_fieldObj;

//! A text input field.

//! \code
//! factory->create_input_field({""}, {20});
//! \endcode
//!
//! \see factory
//!
//! This is a text input field, with a blinking cursor.

typedef ref<input_fieldObj> input_field;

//! A nullable pointer reference to an input_field.

//! \see input_field

typedef ptr<input_fieldObj> input_fieldptr;

//! A reference to a constant input_field object.

//! \see input_field

typedef const_ref<input_fieldObj> const_input_field;

//! A nullable pointer reference to a constant input_field object.

//! \see input_field

typedef const_ptr<input_fieldObj> const_input_fieldptr;

LIBCXXW_NAMESPACE_END

#endif
