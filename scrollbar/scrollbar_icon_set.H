/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef scrollbar_icon_set_h
#define scrollbar_icon_set_h

#include "x/w/namespace.H"
#include "iconfwd.H"
#include "iconobj.H"
#include "connection_threadfwd.H"

LIBCXXW_NAMESPACE_START

//! A set of icons for a \ref scrollbar "scrollbar".

struct LIBCXX_HIDDEN scrollbar_icon_set {

 public:

	//! Scroll in the low direction button.
	icon scroll_low_icon;

	//! Scroll in the high direction button.
	icon scroll_high_icon;

	//! Starting part of the slider handlebar.
	icon handlebar_start_icon;

	//! The middle part of the slider handlebra.
	icon handlebar_icon;

	//! The end part of the slider handlebar.
	icon handlebar_end_icon;

	//! Invoke all icons' initialize() method.
	void initialize(IN_THREAD_ONLY);

	//! Invoke all icons' theme_updated() method.
	void theme_updated(IN_THREAD_ONLY, const defaulttheme &new_theme);
};

LIBCXXW_NAMESPACE_END

#endif
