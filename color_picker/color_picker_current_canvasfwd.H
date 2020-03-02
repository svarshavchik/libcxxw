/*
** Copyright 2019-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_color_picker_current_canvasfwd_h
#define x_w_color_picker_current_canvasfwd_h

#include "x/w/namespace.H"
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN color_picker_current_canvasObj;

/*! The current color picker color

Placeholder widget that shows the currently picked color. It is derived
from the generic canvas object, and draws the currently selected color.

*/

typedef ref<color_picker_current_canvasObj> color_picker_current_canvas;

//! A constant \ref color_picker_current_canvas "current color picker color".

//! \see color_picker_current_canvas

typedef const_ref<color_picker_current_canvasObj> const_color_picker_current_canvas;

//! A nullable pointer reference to a \ref color_picker_current_canvas "current color picker color".

//! \see color_picker_current_canvas

typedef ptr<color_picker_current_canvasObj> color_picker_current_canvasptr;

//! A nullable pointer reference to a const \ref color_picker_current_canvas "current color picker color".

//! \see color_picker_current_canvas

typedef const_ptr<color_picker_current_canvasObj> const_color_picker_current_canvasptr;

LIBCXXW_NAMESPACE_END

#endif
