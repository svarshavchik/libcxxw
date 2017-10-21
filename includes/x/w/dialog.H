/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_dialog_h
#define x_w_dialog_h

#include <x/w/dialogfwd.H>
#include <x/w/dialogobj.H>
#include <x/w/main_window.H>

LIBCXXW_NAMESPACE_START

//! Parameters to \ref dialogObj "dialog public object's constructor".

struct dialog_args {
	ref<dialogObj::implObj> impl;
	ref<layoutmanagerObj::implObj> layout_impl;
};

LIBCXXW_NAMESPACE_END

#endif
