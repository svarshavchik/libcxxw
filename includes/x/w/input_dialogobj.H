/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_input_dialogobj_h
#define x_w_input_dialogobj_h

#include <x/w/input_dialogfwd.H>
#include <x/w/input_fieldfwd.H>
#include <x/w/dialogobj.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see input_dialog

class input_dialogObj : public dialogObj {

public:

	//! The input_field in the dialog.

	const input_field input_dialog_field;

	//! Constructor
	input_dialogObj(const dialog_args &,
			const input_field &) LIBCXX_PUBLIC;

	//! Destructor
	~input_dialogObj() LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
