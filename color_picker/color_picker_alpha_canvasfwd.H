/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_color_picker_alpha_canvasfwd_h
#define x_w_color_picker_alpha_canvasfwd_h

#include "x/w/namespace.H"
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN color_picker_alpha_canvasObj;

/*! Alpha channel gradient canvas


*/

typedef ref<color_picker_alpha_canvasObj> color_picker_alpha_canvas;

//! A constant \ref color_picker_alpha_canvas "alpha channel gradient canvas".

//! \see color_picker_alpha_canvas

typedef const_ref<color_picker_alpha_canvasObj> const_color_picker_alpha_canvas;

//! A nullable pointer reference to a \ref color_picker_alpha_canvas "alpha channel gradient canvas".

//! \see color_picker_alpha_canvas

typedef ptr<color_picker_alpha_canvasObj> color_picker_alpha_canvasptr;

//! A nullable pointer reference to a const \ref color_picker_alpha_canvas "alpha channel gradient canvas".

//! \see color_picker_alpha_canvas

typedef const_ptr<color_picker_alpha_canvasObj> const_color_picker_alpha_canvasptr;

LIBCXXW_NAMESPACE_END

#endif
