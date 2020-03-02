/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_input_dialogfwd_h
#define x_w_input_dialogfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

struct input_dialog_ok_args;

class LIBCXX_PUBLIC input_dialogObj;

/*! A dialog with an input field.

This object is returned by \ref main_window "create_input_dialog()".

It is a subclass of a \ref dialog "dialog object", containing an additional
field, the \ref input_field "input field".

*/

typedef ref<input_dialogObj> input_dialog;

//! A constant \ref input_dialog "input dialog window".

//! \see input_dialog

typedef const_ref<input_dialogObj> const_input_dialog;

//! A nullable pointer reference to a \ref input_dialog "input dialog window".

//! \see input_dialog

typedef ptr<input_dialogObj> input_dialogptr;

//! A nullable pointer reference to a const \ref input_dialog "input dialog window".

//! \see input_dialog

typedef const_ptr<input_dialogObj> const_input_dialogptr;

LIBCXXW_NAMESPACE_END

#endif
