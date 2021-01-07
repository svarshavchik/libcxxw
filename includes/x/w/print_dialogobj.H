/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_print_dialogobj_h
#define x_w_print_dialogobj_h

#include <x/w/print_dialogfwd.H>
#include <x/w/dialogobj.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see print_dialog

class print_dialogObj : public dialogObj {

public:
	class LIBCXX_HIDDEN implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	print_dialogObj(const print_dialog_args &args) LIBCXX_HIDDEN;

	//! Destructor
	~print_dialogObj() LIBCXX_HIDDEN;

	//! Initial dialog open.

	//! Must be used instead of show(). Enumerates available printers,
	//! then show_all()s the dialog.

	void initial_show();
};

LIBCXXW_NAMESPACE_END

#endif
