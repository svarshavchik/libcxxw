/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/input_dialog.H"

LIBCXXW_NAMESPACE_START

input_dialogObj::input_dialogObj(const dialog_args &args,
				 const input_field &input_dialog_field)
	: dialogObj{args},
	  input_dialog_field{input_dialog_field}
{
}

input_dialogObj::~input_dialogObj()=default;

LIBCXXW_NAMESPACE_END
