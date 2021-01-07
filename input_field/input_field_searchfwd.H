/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_input_field_searchfwd_h
#define x_w_input_field_searchfwd_h

#include "x/w/namespace.H"
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN input_field_searchObj;

/*! An input field element that has a search callback.

A subclass of the container that has the editor implementation object.
The subclass is used instead of the stock container when an optional
input_field_search_callback gets specified.

The search popup is attached to the container, and this subclass
pulls in the popup_attachedto_handler_element mixin to position it above
or below this input field.

*/

typedef ref<input_field_searchObj> input_field_search;

//! A constant \ref input_field_search "input field container with an attached search results popup".

//! \see input_field_search

typedef const_ref<input_field_searchObj> const_input_field_search;

//! A nullable pointer reference to a \ref input_field_search "input field container with an attached search results popup".

//! \see input_field_search

typedef ptr<input_field_searchObj> input_field_searchptr;

//! A nullable pointer reference to a const \ref input_field_search "input field container with an attached search results popup".

//! \see input_field_search

typedef const_ptr<input_field_searchObj> const_input_field_searchptr;

LIBCXXW_NAMESPACE_END

#endif
