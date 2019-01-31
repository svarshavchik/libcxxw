/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef popupfwd_h
#define popupfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN popupObj;
class LIBCXX_HIDDEN popup_attachedto_handlerObj;

//! A popup.

//! A top level display element that's not controlled by the window manager.
//! Whatever size and position we want it to be, there it is.
//!
//! The popup typically grabs the pointer or the keyboard focus; but the
//! popup object itself is just a subclass of generic_window that implements
//! metrics notifications by going ahead and reconfiguring itself accordingly.

typedef ref<popupObj> popup;

//! A nullable pointer reference to a popup

typedef ptr<popupObj> popupptr;

LIBCXXW_NAMESPACE_END

#endif
