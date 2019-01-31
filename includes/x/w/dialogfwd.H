/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_dialogfwd_h
#define x_w_dialogfwd_h

#include <x/w/namespace.H>
#include <x/w/factoryfwd.H>
#include <x/w/gridlayoutmanagerfwd.H>
#include <x/w/new_layoutmanagerfwd.H>
#include <x/w/rectangle.H>
#include <x/w/screen_positionsfwd.H>
#include <x/functionalrefptrfwd.H>
#include <x/ptrfwd.H>

#include <unordered_map>
#include <string>
#include <functional>
#include <optional>
#include <string_view>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC dialogObj;

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

//! Parameters for a new dialog.

//! Passed to \ref main_window "main window"'s methods that create new
//! dialogs, specifies the new dialog's configuration and parameters.

struct standard_dialog_args {

	//! Unique dialog ID.

	//! Cannot be set to a temporary object, this is a \c std::string_view.
	std::string_view dialog_id;

	//! Whether the new dialog is a modal dialog.
	bool modal=false;

	//! Window ID
	std::string window_id;

	//! Default dialog position
	std::variant<dialog_position, rectangle> position=
		dialog_position::default_position;

	//! Set the urgent flag on the dialog.

	//! Some window managers draw attention, in some way, to urgent
	//! dialogs. This sets the urgent flag on the new dialog.
	bool urgent=false;

	//! Whether the new dialog grabs input focus when it opens.
	bool grab_input_focus=true;

	//! Default dialog position.
	void screen_position(dialog_position) LIBCXX_PUBLIC;

	//! Restore the dialog's previous screen position.
	void screen_position(const screen_positions &) LIBCXX_PUBLIC;

	//! Restore the dialog's previous screen position.
	void screen_position(const screen_positions &,
			     const std::string_view &) LIBCXX_PUBLIC;
};

//! An additional parameter for create_dialog() and create_custom_dialog().

//! Passed to \ref main_window "main window"'s create_dialog() or
//! create_custom_dialog(), setting the new dialog's parameters.
//!
//! A subclass of \ref standard_dialog_args "INSERT_LIBX_NAMESPACE::w::standard_dialog_args"
//! that adds an optional \c dialog_layout, the layout manager for the new
//! dialog.

struct create_dialog_args : public standard_dialog_args {

	//! New dialog's layout manager.

	//! \code
	//! INSERT_LIBX_NAMESPACE::w::create_dialog_args args{"example@example.com"};
	//!
	//! INSERT_LIBX_NAMESPACE::w::new_toolboxlayoutmanager ntlm;
	//!
	//! args.dialog_layout=ntlm;
	//! \endcode
	//!
	//! The new layout manager object cannot be a temporary object, and
	//! the object must exist until create_dialog() or
	//! create_custom_dialog() returns.

	std::optional<std::reference_wrapper<const new_layoutmanager>
		      > dialog_layout;
};

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
