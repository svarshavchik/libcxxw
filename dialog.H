/*
** Copyright 2017 Double Precision, Inc.
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
#include <functional>
#include <utility>

LIBCXXW_NAMESPACE_START

typedef decltype(make_weak_capture(std::declval<dialog &&>())
		 ) captured_dialog_t;


void hide_and_invoke(const captured_dialog_t &d,
		     const busy &yes_i_am,
		     const std::function<void (const busy &)>  &action);

void hide_and_invoke_when_activated(const dialog &d,
				    const hotspot &button,
				    const std::function
				    <void (const busy &)> &action);

void hide_and_invoke_when_closed(const dialog &d,
				 const std::function
				 <void (const busy &)> &action);

std::function<void (const factory &)>
dialog_ok_button(const text_param &label,
		 buttonptr &ret,
		 char32_t key);

std::function<void (const factory &)>
dialog_cancel_button(const text_param &label,
		     buttonptr &ret,
		     char32_t key);

std::function<void (const factory &)> dialog_filler();

LIBCXXW_NAMESPACE_END

#endif
