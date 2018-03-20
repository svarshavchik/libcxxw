/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef popup_attachedto_handlerfwd_h
#define popup_attachedto_handlerfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class popup_attachedto_handlerObj;

struct popup_attachedto_handler_args;

/*! A handler for a popup that's attached to an element.

Subclass of popupObj::handlerObj implementing recalculate_popup_position()
to attach the popup to another element.

*/

typedef ref<popup_attachedto_handlerObj> popup_attachedto_handler;

//! A constant \ref popup_attachedto_handler "element-attached popup".

//! \see popup_attachedto_handler

typedef const_ref<popup_attachedto_handlerObj> const_popup_attachedto_handler;

//! A nullable pointer reference to a \ref popup_attachedto_handler "element-attached popup".

//! \see popup_attachedto_handler

typedef ptr<popup_attachedto_handlerObj> popup_attachedto_handlerptr;

//! A nullable pointer reference to a const \ref popup_attachedto_handler "element-attached popup".

//! \see popup_attachedto_handler

typedef const_ptr<popup_attachedto_handlerObj> const_popup_attachedto_handlerptr;

LIBCXXW_NAMESPACE_END

#endif
