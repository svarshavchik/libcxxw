/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_dialogfwd_h
#define x_w_dialogfwd_h

#include <x/w/namespace.H>
#include <x/w/factoryfwd.H>
#include <x/functionalrefptrfwd.H>
#include <x/ptrfwd.H>

#include <unordered_map>
#include <string>
#include <functional>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC dialogObj;

/*! A dialog pop-up from another main window (or of a parent dialog).

A dialog gets created by using one of main_window's methods, like
create_ok_cancel_dialog():

\code
#include <INSERT_LIBX_NAMESPACE/w/main_window.H>
#include <INSERT_LIBX_NAMESPACE/w/dialog.H>

INSERT_LIBX_NAMESPACE::w::main_window new_main_window=INSERT_LIBX_NAMESPACE::w::main_window::create(
     []
     (const INSERT_LIBX_NAMESPACE::w::main_window &new_main_window)
     {
         // Creator lambda
     });

// In the creator lambda, or separately:

INSERT_LIBX_NAMESPACE::w::dialog d=help->create_ok_cancel_dialog(
    "help@example.com",
    "alert",
    []
    (const INSERT_LIBX_NAMESPACE::w::factory &factory)
    {
         factory->create_label("Help yoruself");
   },
    (const INSERT_LIBX_NAMESPACE::w::busy &ignore)
    {
         // Ok action.
    },
    (const INSERT_LIBX_NAMESPACE::w::busy &ignore)
    {
         // Cancel action.
    },
    true);

d->dialog_window->set_window_title("Help");

// To make the dialog visible.
d->dialog_window->show_all();
\endcode

A dialog itself is like a top level display element, appearing on the
desktop like a \ref main_window "main_window". In fact, the class contains
a \c dialog_window member which is the dialog's
\ref main_window "INSERT_LIBX_NAMESPACE::w::main_window".
This means that each dialog can have its own dialogs, too.

Several INSERT_LIBX_NAMESPACE::w::main_window methods, like
create_ok_cancel_dialog() construct stock, theme-specified dialogs.

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
			   functionref<void (const factory &)>
			   > standard_dialog_elements_t;

LIBCXXW_NAMESPACE_END

#endif
