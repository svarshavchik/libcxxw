/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_color_picker_squarefwd_h
#define x_w_color_picker_squarefwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class color_picker_squareObj;

/*! Color square for the color picker.

Rectangle of fixed dimensions, drawn with a 2D gradient. One of the R, G, and
B components is fixed, and the other two components are used as gradients
horizontally and vertically.

*/

typedef ref<color_picker_squareObj> color_picker_square;

//! A constant \ref color_picker_square "color picker gradient square".

//! \see color_picker_square

typedef const_ref<color_picker_squareObj> const_color_picker_square;

//! A nullable pointer reference to a \ref color_picker_square "color picker gradient square".

//! \see color_picker_square

typedef ptr<color_picker_squareObj> color_picker_squareptr;

//! A nullable pointer reference to a const \ref color_picker_square "color picker gradient square".

//! \see color_picker_square

typedef const_ptr<color_picker_squareObj> const_color_picker_squareptr;

LIBCXXW_NAMESPACE_END

#endif
