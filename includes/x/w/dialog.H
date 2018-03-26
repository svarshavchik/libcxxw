/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_dialog_h
#define x_w_dialog_h

#include <x/w/dialogfwd.H>
#include <x/w/dialogobj.H>
#include <x/w/main_window.H>
#include <x/functionalrefptr.H>

LIBCXXW_NAMESPACE_START

//! Parameters to \ref dialogObj "dialog public object's constructor".

struct dialog_args {
	ref<dialogObj::implObj> impl;
	main_window dialog_window;
};

LIBCXXW_NAMESPACE_END

#endif
