/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_color_pickerfwd_h
#define x_w_color_pickerfwd_h

#include <x/w/namespace.H>
#include <x/w/connection_threadfwd.H>
#include <x/w/rgbfwd.H>
#include <x/w/callback_triggerfwd.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class color_pickerObj;

/*! Color picker display element.

*/

typedef ref<color_pickerObj> color_picker;

//! A constant \ref color_picker "color picker".

//! \see color_picker

typedef const_ref<color_pickerObj> const_color_picker;

//! A nullable pointer reference to a \ref color_picker "color picker".

//! \see color_picker

typedef ptr<color_pickerObj> color_pickerptr;

//! A nullable pointer reference to a const \ref color_picker "color picker".

//! \see color_picker

typedef const_ptr<color_pickerObj> const_color_pickerptr;

//! Signature for the color picker callback.

typedef void color_picker_callback_t(THREAD_CALLBACK,
				     const rgb &new_color,
				     const callback_trigger_t &trigger,
				     const busy &mcguffin);

LIBCXXW_NAMESPACE_END

#endif
