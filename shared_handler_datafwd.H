/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef shared_handler_datafwd_h
#define shared_handler_datafwd_h

#include "x/w/namespace.H"
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN shared_handler_dataObj;

/*! Data shared between top level window handlers.

A single object that's referenced by both the top level main window handler
and all of its popups' top level window handlers.

Keeps track of all opened popups in a window.

Popups: exclusive dropdown popup (combo-boxes, date picker popups), and menu
popups.

This object gets created and inserted into the main window handler object
(generic_windowObj::handlerObj), then gets linked to each popup handler object
created from that window, so that all popups have access to it.

Keeps track of all installed shortcuts from the main window and all popups.

So that a shortcut for a menu item will activate it, even when the menu item
is closed.

*/

typedef ref<shared_handler_dataObj> shared_handler_data;

//! A constant \ref shared_handler_data "WHATITIS".

//! \see shared_handler_data

typedef const_ref<shared_handler_dataObj> const_shared_handler_data;

//! A nullable pointer reference to a \ref shared_handler_data "WHATITIS".

//! \see shared_handler_data

typedef ptr<shared_handler_dataObj> shared_handler_dataptr;

//! A nullable pointer reference to a const \ref shared_handler_data "WHATITIS".

//! \see shared_handler_data

typedef const_ptr<shared_handler_dataObj> const_shared_handler_dataptr;

LIBCXXW_NAMESPACE_END

#endif
