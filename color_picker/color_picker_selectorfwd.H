/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_color_picker_selectorfwd_h
#define x_w_color_picker_selectorfwd_h

#include "x/w/namespace.H"
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class color_picker_selectorObj;

/*! Container in the color picker popup.

The container with all the colored goodies.

*/

typedef ref<color_picker_selectorObj> color_picker_selector;

//! A constant \ref color_picker_selector "color picker popup container".

//! \see color_picker_selector

typedef const_ref<color_picker_selectorObj> const_color_picker_selector;

//! A nullable pointer reference to a \ref color_picker_selector "color picker popup container".

//! \see color_picker_selector

typedef ptr<color_picker_selectorObj> color_picker_selectorptr;

//! A nullable pointer reference to a const \ref color_picker_selector "color picker popup container".

//! \see color_picker_selector

typedef const_ptr<color_picker_selectorObj> const_color_picker_selectorptr;

LIBCXXW_NAMESPACE_END

#endif
