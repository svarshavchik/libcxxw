/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_current_selection_paste_handlerfwd_h
#define x_w_current_selection_paste_handlerfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class current_selection_paste_handlerObj;

/*! Implement a conversion handler for text.

Implements \ref current_selection_handler "current_selection_handler" for
text selections. The text is processed by invoking pasted_string().

The conversion_faied() callback is not implemented. It must be implemented
in a subclass.

*/

typedef ref<current_selection_paste_handlerObj> current_selection_paste_handler;

//! A constant \ref current_selection_paste_handler "text conversion handler".

//! \see current_selection_paste_handler

typedef const_ref<current_selection_paste_handlerObj> const_current_selection_paste_handler;

//! A nullable pointer reference to a \ref current_selection_paste_handler "text conversion handler".

//! \see current_selection_paste_handler

typedef ptr<current_selection_paste_handlerObj> current_selection_paste_handlerptr;

//! A nullable pointer reference to a const \ref current_selection_paste_handler "text conversion handler".

//! \see current_selection_paste_handler

typedef const_ptr<current_selection_paste_handlerObj> const_current_selection_paste_handlerptr;

LIBCXXW_NAMESPACE_END

#endif
