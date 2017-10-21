/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_dialogfwd_h
#define x_w_dialogfwd_h

#include <x/w/namespace.H>
#include <x/w/gridfactoryfwd.H>
#include <x/ptrfwd.H>

#include <unordered_map>
#include <string>
#include <functional>

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

struct dialog_args;

//! Standard dialog factories

//! An internal data structure that represents all factories for a particular
//! theme-specified dialog.

typedef std::unordered_map<std::string,
			   std::function<void (const gridfactory &)>
			   > standard_dialog_elements_t;

LIBCXXW_NAMESPACE_END

#endif
