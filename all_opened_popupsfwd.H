/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef all_opened_popupsfwd_h
#define all_opened_popupsfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class all_opened_popupsObj;

/*! Keep track of all opened popups in a window.

Popups: combo-box dropdown popup, and menu popups.

This object gets created and inserted into the main window handler object
(generic_windowObj::handlerObj), then gets linked to each popup handler object
created from that window, so that all popups have access to it.

*/

typedef ref<all_opened_popupsObj> all_opened_popups;

//! A constant \ref all_opened_popups "WHATITIS".

//! \see all_opened_popups

typedef const_ref<all_opened_popupsObj> const_all_opened_popups;

//! A nullable pointer reference to a \ref all_opened_popups "WHATITIS".

//! \see all_opened_popups

typedef ptr<all_opened_popupsObj> all_opened_popupsptr;

//! A nullable pointer reference to a const \ref all_opened_popups "WHATITIS".

//! \see all_opened_popups

typedef const_ptr<all_opened_popupsObj> const_all_opened_popupsptr;

LIBCXXW_NAMESPACE_END

#endif
