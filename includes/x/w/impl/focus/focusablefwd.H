/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_focusablefwd_h
#define x_w_impl_focusablefwd_h

#include <x/ptrfwd.H>
#include <x/w/focusable.H>
#include <x/w/namespace.H>
#include <list>

LIBCXXW_NAMESPACE_START

//! A list of all focusable fields.

typedef std::list<focusable_impl> focusable_fields_t;

//! The purpose of calling enabled()

enum class enabled_for {

	//! Whether the widget can receive keyboard and pointer input
	input_focus,

	//! Whether to activate the shortcut related to this widget.

	//! Even if this widget is enabled, if it's in a popup menu that's
	//! attached to a widget that's not visible or cannot receive input
	//! focus this keyboard shortcut is not active.
	shortcut_activation
};

LIBCXXW_NAMESPACE_END

#endif
