/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_dialogfwd_h
#define x_w_dialogfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class dialogObj;

/*! A dialog of another main window (or dialog).

Dialogs own references on their parent dialog or main window.

*/

typedef ref<dialogObj> dialog;

//! A constant \ref dialog "dialog window".

//! \see dialog

typedef const_ref<dialogObj> const_dialog;

//! A nullable pointer reference to a \ref dialog "dialog window".

//! \see dialog

typedef ptr<dialogObj> dialogptr;

//! A nullable pointer reference to a const \ref dialog "dialog window".

//! \see dialog

typedef const_ptr<dialogObj> const_dialogptr;

LIBCXXW_NAMESPACE_END

#endif
