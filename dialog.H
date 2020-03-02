/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef dialog_h
#define dialog_h

#include "x/w/dialog.H"
#include "x/w/factoryfwd.H"
#include "x/w/text_paramfwd.H"
#include "x/w/buttonfwd.H"
#include "x/w/busyfwd.H"
#include "x/w/hotspotfwd.H"

#include <x/weakcapture.H>
#include <x/functionalrefptr.H>
#include <utility>

LIBCXXW_NAMESPACE_START

struct LIBCXX_HIDDEN dialog_args;

//! Parameters to \ref dialogObj "dialog public object's constructor".

struct dialog_args {
	ref<dialogObj::implObj> impl;
	main_window dialog_window;
};

void hide_and_invoke_when_activated(const main_window &parent_window,
				    const dialog &d,
				    const hotspot &button,
				    const ok_cancel_dialog_callback_t &action)
	LIBCXX_PUBLIC;

void hide_and_invoke_when_closed(const main_window &parent_window,
				 const dialog &d,
				 const ok_cancel_dialog_callback_t &action)
	LIBCXX_PUBLIC;

functionref<void (const factory &)>
dialog_ok_button(const text_param &label,
		 buttonptr &ret,
		 char32_t key);

functionref<void (const factory &)>
dialog_cancel_button(const text_param &label,
		     buttonptr &ret,
		     char32_t key);

LIBCXXW_NAMESPACE_END

#endif
