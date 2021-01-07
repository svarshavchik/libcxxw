/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_input_dialog_h
#define x_w_input_dialog_h

#include <x/w/input_dialogfwd.H>
#include <x/w/input_dialogobj.H>
#include <x/w/dialog.H>
#include <x/w/input_field.H>

LIBCXXW_NAMESPACE_START

//! Parameter passed to \ref main_window "standard input dialog's" ok callback.

struct input_dialog_ok_args : public ok_cancel_callback_args {

	//! The dialog's input field.
	input_field dialog_input_field;
};

LIBCXXW_NAMESPACE_END

#endif
