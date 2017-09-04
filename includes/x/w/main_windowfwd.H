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
//! \par Close button
//!
//! \code
//! main_window->on_delete([]
//!                        (const INSERT_LIBX_NAMESPACE::w::busy &block)
//!                        {
//!                        });
//! \endcode
//!
//! on_delete() installs a lambda that gets executed when the main application
//! window's close button gets clicked. The closure receives a
//! \ref busy "busy blocking" object.
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
//! Creates a custom dialog. The dialog needs to be show()n. The second optional
//! parameter specifies the new dialog's layout manager, and defaults to
//! \ref new_gridlayoutmanager "INSERT_LIBX_NAMESPACE::w::new_gridlayoutmanager".
//! The third optional parameter is a \c bool flag. Setting the flag to \c true
//! creates a modal dialog. While a modal dialog is visible, the parent
//! window or dialog ignores all pointer and button events, and gets visibly
//! shaded accordingly.
//!
//! Normally, parent display elements in a window own child elements they
//! contain. Dialogs own their parent windows. Do not store dialogs directly
//! or indirectly in the parent window's \c appdata, or any display element,
//! as this creates a circular reference.
//!
//! \code
//! auto d=create_standard_dialog("ok-dialog", true, {
//!     {"icon", icon_element(icon)},
//!     {"message", content_factory},
//!     {"ok", create_ok_button(ok_label, ok_button, '\n')}
//! });
//! \endcode
//!
//! create_standard_dialog() constructs a dialog whose layout is
//! specified by the current display theme. This method is mostly for internal
//! use. The first parameter is the name of the dialog layout defined by the
//! current theme. The second parameter sets whether the new dialog is modal.
//! The third parameter is a
//! \c std::unordered_map<std::string, std::function<void (const INSERT_LIBX_NAMESPACE::w::gridfactory &)>>.
//! This is a map of factories for individual display elements. The theme
//! layout refers to the display elements in the dialog by an internal label
//! which comes from the unordered map's keys. The keys value is a callback
//! that constructs the corresponding display element. The callback receives
//! the parameter to the \ref gridfactory "grid factory" which the callback
//! must invoke to construct the corresponding display element.
//!
//! create_standard_dialog() uses the map, and invokes the callbacks. The
//! map and the callbacks do not get used or referenced after
//! create_standard_dialog() returns. Use \c show_all() to make the returned
//! callback visible.
//!
//! \code
//! INSERT_LIBX_NAMESPACE::w::dialog d=w->create_ok_dialog("alert",
//!     []
//!     (const INSERT_LIBX_NAMESPACE::w::gridfactory &factory)
//!     {
//!          factory->create_label("Lorem ipsum");
//!     },
//!     (const INSERT_LIBX_NAMESPACE::w::busy &ignore)
//!     {
//!          // ...
//!     }, true);
//!
//! INSERT_LIBX_NAMESPACE::w::dialog d=w->create_ok_cancel_dialog("alert",
//!     []
//!     (const INSERT_LIBX_NAMESPACE::w::gridfactory &factory)
//!     {
//!          factory->create_label("Lorem ipsum");
//!     },
//!     (const INSERT_LIBX_NAMESPACE::w::busy &ignore)
//!     {
//!          // Ok action.
//!     },
//!     (const INSERT_LIBX_NAMESPACE::w::busy &ignore)
//!     {
//!          // Cancel action.
//!     },
//!     true);
//!
//! \endcode
//!
//! create_ok_dialog() creates a dialog with only an "Ok" button.
//! create_cancel_dialog() creates a dialog with "Ok" and "Cancel" buttons.
//! Selecting either button hide()s the dialog and invokes the corresponding
//! callback lambda.
//!
//! The dialog shows an icon on its left side, the first parameter specifies
//! one of the predefined icon names from the current theme. The second
//! parameter is a callback lambda that creates the main contents of the
//! dialog to show next to the icon, typically a short label.
//!
//! The last optional parameter specifies whether the created dialog is
//! modal.
//!
//! \c show_all() makes the returned dialog visible.

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
