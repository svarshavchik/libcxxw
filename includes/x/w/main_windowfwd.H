/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_main_windowfwd_h
#define x_w_main_windowfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class main_windowObj;
class main_windowBase;

//! Top level application window.
//!
//! \code
//! // Create an application window on the default screen.
//!
//! INSERT_LIBX_NAMESPACE::w::main_window main_window=INSERT_LIBX_NAMESPACE::w::main_window::create(
//!      []
//!      (const INSERT_LIBX_NAMESPACE::w::main_window &new_main_window)
//!      {
//!           // Initialize the new main window.
//!      });
//!
//! // Explicitly create an application window on a specific screen.
//!
//! auto screen=INSERT_LIBX_NAMESPACE::w::screen::create();
//!
//! INSERT_LIBX_NAMESPACE::w::main_window main_window=screen->create_mainwindow(
//!                 []
//!                 (const INSERT_LIBX_NAMESPACE::w::mainwindow &w)
//!                 {
//!                        // Initialize the new main window.
//!                 });
//!
//! \endcode
//!
//! Like \ref factory "factories", creating a top level application window
//! uses a creator lambda to populate its contents. The creator
//! lambda's parameter is the new main window.
//!
//! Main windows inherit from \ref generic_window "generic windows".
//!
//! \par Menu bar
//!
//! \code
//! INSERT_LIBX_NAMESPACE::w::menubarlayoutmanager mb=w->get_menubarlayoutmanager();
//!
//! INSERT_LIBX_NAMESPACE::w::container c=w->get_menubar();
//!
//! c->show();
//! \endcode
//!
//! The menu bar at the top of the main window is initially hidden.
//! get_menubar() returns its \ref container "container", whose layout manager
//! is \ref menubarlayoutmanager "INSERT_LIBX_NAMESPACE::w::menubarlayoutmanager".
//! get_menubarlayoutmanager() is a equivalent to
//! \c get_menubar()->get_layoutmanager().
//!
//! \par Dialogs
//!
//! \code
//! INSERT_LIBX_NAMESPACE::w::dialog d=w->create_dialog([]
//!      (const INSERT_LIBX_NAMESPACE::w::dialog &f)
//!      {
//!      });
//! \endcode
//!
//! Creates a dialog. The dialog needs to be show()n. The second optional
//! parameter specifies the new dialog's layout manager, and defaults to
//! \ref new_gridlayoutmanager "INSERT_LIBX_NAMESPACE::w::new_gridlayoutmanager".
//! The third optional parameter is a \bool flag. Setting it to \c true
//! creates a modal dialog. While a modal dialog is visible, the parent
//! window or dialog ignores all pointer and button events, and gets visibly
//! shaded accordingly.
//!
//! Normally, parent display elements in a window own child elements they
//! contain. Dialogs own their parent windows. Do not store dialogs directly
//! or indirectly in the parent window's \c appdata, or any display element,
//! as this creates a circular reference.

typedef ref<main_windowObj, main_windowBase> main_window;

//! A reference to a constant main application window.

//! \see main_window

typedef const_ref<main_windowObj, main_windowBase> const_main_window;

//! A nullable pointer reference to a main application window.

//! \see main_window

typedef ptr<main_windowObj, main_windowBase> main_windowptr;

//! A nullable pointer reference to a constant main application window.

//! \see main_window

typedef const_ptr<main_windowObj, main_windowBase> const_main_windowptr;

LIBCXXW_NAMESPACE_END

#endif
