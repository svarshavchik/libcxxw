/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_input_field_search_popup_handlerfwd_h
#define x_w_input_field_search_popup_handlerfwd_h

#include "x/w/namespace.H"
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN input_field_search_popup_handlerObj;

/*! Popup handler for the optional input field search functionality.

The handler for a popup that gets created when an input field has an
input_field_search_callback.

*/

typedef ref<input_field_search_popup_handlerObj> input_field_search_popup_handler;

//! A constant \ref input_field_search_popup_handler "input search popup handler".

//! \see input_field_search_popup_handler

typedef const_ref<input_field_search_popup_handlerObj> const_input_field_search_popup_handler;

//! A nullable pointer reference to a \ref input_field_search_popup_handler "input search popup handler".

//! \see input_field_search_popup_handler

typedef ptr<input_field_search_popup_handlerObj> input_field_search_popup_handlerptr;

//! A nullable pointer reference to a const \ref input_field_search_popup_handler "input search popup handler".

//! \see input_field_search_popup_handler

typedef const_ptr<input_field_search_popup_handlerObj> const_input_field_search_popup_handlerptr;

LIBCXXW_NAMESPACE_END

#endif
