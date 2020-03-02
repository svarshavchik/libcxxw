/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/input_dialog.H"
#include "x/w/button.H"

LIBCXXW_NAMESPACE_START

input_dialogObj::input_dialogObj(const dialog_args &args,
				 const input_field &input_dialog_field,
				 const button &input_dialog_ok)
	: dialogObj{args},
	  input_dialog_field{input_dialog_field},
	  input_dialog_ok{input_dialog_ok}
{
}

input_dialogObj::~input_dialogObj()=default;

LIBCXXW_NAMESPACE_END
