/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef custom_combobox_popupfwd_h
#define custom_combobox_popupfwd_h

#include "x/w/namespace.H"
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class custom_combobox_popupObj;

//! Combo-box popup window handler.

//! Subclass of popup_attachedto_handlerObj, the window handler display
//! element for popups.

typedef ref<custom_combobox_popupObj> custom_combobox_popup;

LIBCXXW_NAMESPACE_END

#endif
