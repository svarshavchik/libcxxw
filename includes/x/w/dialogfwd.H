/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_dialogfwd_h
#define x_w_dialogfwd_h

#include <x/w/namespace.H>
#include <x/w/main_windowfwd.H>
#include <x/w/busyfwd.H>
#include <x/w/factoryfwd.H>
#include <x/w/gridlayoutmanagerfwd.H>
#include <x/w/new_layoutmanagerfwd.H>
#include <x/w/rectangle.H>
#include <x/w/rgb.H>
#include <x/w/font_arg.H>
#include <x/w/screen_positionsfwd.H>
#include <x/w/connection_threadfwd.H>
#include <x/functionalrefptrfwd.H>
#include <x/ptrfwd.H>

#include <unordered_map>
#include <string>
#include <functional>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC dialogObj;

//! Parameters passed to the ok/cancel callback of a standard dialog.

//! Callbacks from a standard dialog created by
//! \ref main_window "main window's" create_ok_dialog(),
//! create_ok_cancel_dialog(), et. al., receive this parameter.

struct ok_cancel_callback_args {

	//! The dialog's parent window

	//! \note
	//! This may be a null ptr if the main window gets destroyed without
	//! destroying the dialog, first.

	main_windowptr dialog_main_window;

	//! The busy mcguffin is available to the callback.
	const busy &mcguffin;
};

//! Signature of an ok or cancel callback.

typedef functionref<void(THREAD_CALLBACK,
			 const ok_cancel_callback_args &)
		    > ok_cancel_dialog_callback_t;

//! Position of a new dialog with respect to its parent window

enum class dialog_position
	{
	 default_position, //!< Default dialog position
	 on_the_left,	//!< Position the dialog to the left of the main window
	 on_the_right,	//!< Position the dialog to the right of the main window
	 above,	//!< Position the dialog above the main window
	 below, //!< Position the dialog below the main window
	};

struct LIBCXX_PUBLIC standard_dialog_args;
struct LIBCXX_PUBLIC create_dialog_args;

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

A dialog itself is like a top level widget, appearing on the
desktop like a \ref main_window "main_window". In fact, the class contains
a \c dialog_window member which is the dialog's
\ref main_window "INSERT_LIBX_NAMESPACE::w::main_window".
This means that each dialog can have its own dialogs, too.

Several INSERT_LIBX_NAMESPACE::w::main_window methods, like
create_ok_cancel_dialog() construct stock, theme-specified dialogs.

\code
d->set_dialog_position(INSERT_LIBX_NAMESPACE::w::dialog_position::on_the_left);
\endcode

set_dialog_position() specifies where the dialog should appear with
respect to its parent main window. This is effective only before the dialog
is shown for the first time; or while the dialog is hidden, and its position
and size gets reset to this default position.

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
