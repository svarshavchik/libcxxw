/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_main_windowfwd_h
#define x_w_main_windowfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

#include <unordered_map>
#include <string>
#include <variant>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC main_windowObj;
class LIBCXX_PUBLIC main_windowBase;
class LIBCXX_PUBLIC main_window_config;
class LIBCXX_PUBLIC splash_window_config;
class LIBCXX_PUBLIC transparent_splash_window_config;
typedef std::variant<main_window_config,
		     splash_window_config,
		     transparent_splash_window_config> main_window_config_t;

/*! Top level application window.

Inherits some functionality from its \ref generic_window "generic_window"
superclass.

\code
// Create an application window on the default screen.

INSERT_LIBX_NAMESPACE::w::main_window main_window=INSERT_LIBX_NAMESPACE::w::main_window::create(
     []
     (const INSERT_LIBX_NAMESPACE::w::main_window &new_main_window)
     {
          // Initialize the new main window.
     });

// Explicitly create an application window on a specific screen.

auto screen=INSERT_LIBX_NAMESPACE::w::screen::create();

INSERT_LIBX_NAMESPACE::w::main_window main_window=screen->create_mainwindow(
                []
                (const INSERT_LIBX_NAMESPACE::w::mainwindow &w)
                {
                       // Initialize the new main window.
                });
\endcode

Like \ref factory "factories", creating a top level application window
uses a creator lambda to populate its contents. The creator
lambda's parameter is the new main window.

Main windows inherit from \ref generic_window "generic windows".

\see screen_positions

\par Window icon

\code
main_window->install_window_icons({"appsmall.gif", "appmedium.gif", "applarge.gif"});
main_window->install_window_icons("app.sxg");

\endcode

install_window_icons() sets icon shown in the main window's
title bar. install_window_icons()'s parameter is a vector of strings.
Multiple icons of different sizes should be made available, for the window
manager to pick an icon with a preferred size.

Theme icons are .sxg-formatted images that are scalable to multiple sizes.
An overloaded
install_window_icons() takes a single filename referring to an .sxg file,
and installs multiple copies of it that are scaled to preset default sizes.

Filenames without a search component gets searched for in the current
directory, and the current display theme's directory if the file does
not exist in the current directory.

The filename suffix is optional and can be omitted;
install_window_icons() tries each known filename suffix.

\par Close button

\code
main_window->on_delete([]
                       (ONLY IN_THREAD,
                        const INSERT_LIBX_NAMESPACE::w::busy &block)
                       {
                       });
\endcode

on_delete() installs a lambda that gets executed when the main application
window's close button gets clicked. The closure receives a
\ref busy "busy blocking" object.

\par Menu bar

\code
INSERT_LIBX_NAMESPACE::w::menubarlayoutmanager mb=w->get_menubarlayoutmanager();

INSERT_LIBX_NAMESPACE::w::container c=w->get_menubar();

c->show();
\endcode

The menu bar at the top of the main window is initially hidden.
get_menubar() returns its \ref container "container", whose layout manager
is \ref menubarlayoutmanager "INSERT_LIBX_NAMESPACE::w::menubarlayoutmanager".
get_menubarlayoutmanager() is a equivalent to
\c get_menubar()->get_layoutmanager().

\par Dialogs

\code
INSERT_LIBX_NAMESPACE::w::dialog d=w->create_dialog("dialog_id@example.com",
     []
     (const INSERT_LIBX_NAMESPACE::w::dialog &f)
     {
     });

INSERT_LIBX_NAMESPACE::w::create_dialog_args args{"dialog_id@example.com"};

INSERT_LIBX_NAMESPACE::w::dialog d=w->create_dialog(args,
     []
     (const INSERT_LIBX_NAMESPACE::w::dialog &f)
     {
     });

d->dialog_window->show_all();
\endcode

Creates a generic dialog. The dialog needs to be show_all()ed, to make
it visible by showing all of the dialog's widgets.

The first parameter is an
\ref create_dialog_args "INSERT_LIBX_NAMESPACE::w::create_dialog_args"
object. This object sets the new dialog's parameters. It's constructible
from a single literal string, or this object is individually constructible,
and passed in. Its required first
parameter is a unique identifier for the new dialog. All dialogs created
by the same main window must have unique identifiers. By convention,
unique dialog identifiers are based on Internet domain names, either
"unique.example.com", or "unique@example.com", where "example.com" is
an Internet domain registered by the application.

The second parameter is the new dialog's creator, a lambda that gets
called just before
create_dialog() returns. It receives the just-created dialog as its
parameter, and its job is to create the dialog's contents.

Optional
\ref create_dialog_args "INSERT_LIBX_NAMESPACE::w::create_dialog_args"
parameters:

\code
    args.modal=true;

INSERT_LIBX_NAMESPACE::w::dialog d=w->create_dialog({"dialog_id@example.com", true},
     []
     (const INSERT_LIBX_NAMESPACE::w::dialog &f)
     {
     });
\endcode

\c modal is
a \c bool flag. It's \c false by default. Setting it to \c true
creates a modal dialog. While a modal dialog is visible, the parent
window or dialog ignores all pointer and button events, and gets visibly
shaded accordingly.

\code
INSERT_LIBX_NAMESPACE::w::new_toolboxlayoutmanager ntlm;

args.dialog_layout=ntlm;
\endcode

\ref create_dialog_args "INSERT_LIBX_NAMESPACE::w::create_dialog_args"'s
\c dialog_layout member specifies the new dialog's layout manager.
It defaults to
\ref new_gridlayoutmanager "INSERT_LIBX_NAMESPACE::w::new_gridlayoutmanager".

If specified, \c dialog_layout cannot be set to a temporary object, and the
new layout manager object cannot be destroyed before the new dialog gets
created.

\par Standard dialogs

\code
INSERT_LIBX_NAMESPACE::w::dialog d=w->create_ok_dialog(
    // INSERT_LIBX_NAMESPACE::w::standard_dialog_args
    //
    // Same as INSERT_LIBX_NAMESPACE::w::create_dialog_args except that there's
    // no dialog_layout.
    //
    // Like INSERT_LIBX_NAMESPACE::w::create_dialog_args, it's constructible
    // from a single literal string, and also has an optional modal flag;
    // the literal string sets a unique identifier, or a label, for the dialog.
    //
    // All dialogs must have unique labels in the parent windows or dialogs.
    // Different parent main window or parent dialogs can use the same labels.
    // For future use, identifier labels should be formatted as Internet
    // hostnames or E-mail addresses using domain names that belong to
    // the application.

    {"lorem_ipsum@example.com", true},

    // Use this icon for this dialog.
    "alert",

    // The featured widget.
    []
    (const INSERT_LIBX_NAMESPACE::w::factory &factory)
    {
         factory->create_label("Lorem ipsum");
    },

    // The callback when the dialog gets closed.
    []
    (ONLY IN_THREAD,
     const INSERT_LIBX_NAMESPACE::w::ok_cancel_callback_args &args)
    {
         // ...
    });

INSERT_LIBX_NAMESPACE::w::dialog d=w->create_ok_cancel_dialog(
    // Just a dialog label, the modal flag defaults to false.
    "dolores@example.com",

    // Icon
    "alert",

    // The featured widget.
    []
    (const INSERT_LIBX_NAMESPACE::w::factory &factory)
    {
         factory->create_label("Lorem ipsum");
   },
    (ONLY IN_THREAD,
     const INSERT_LIBX_NAMESPACE::w::ok_cancel_callback_args &args)
    {
         // Ok action.
    },
    (ONLY IN_THREAD,
     const INSERT_LIBX_NAMESPACE::w::ok_cancel_callback_args &args)
    {
         // Cancel action.
    });
\endcode

create_ok_dialog() creates a \ref dialog "dialog" with only an "Ok" button.
create_ok_cancel_dialog() creates a dialog with "Ok" and "Cancel" buttons.
Selecting either button hide()s the dialog and invokes the corresponding
callback lambda.

The dialog shows an icon on its left side, the second parameter specifies
one of the predefined icon names from the current theme. The second
parameter is a callback lambda that creates the main contents of the
dialog to show next to the icon, typically a short label.

\code
INSERT_LIBX_NAMESPACE::w::input_dialog d=main_window->create_input_dialog
      (// Identifier label, modal dialog.
       {"help_question@example.com", true},

       // Show an icon with a question mark.
       "question",

       // Callback: the element to the left of the input field.
       []
       (const INSERT_LIBX_NAMESPACE::w::factory &f)
       {
             f->create_label("What is your name?");
       },

       // Initial value of the input field
       "",

       // Input field's configuration
       INSERT_LIBX_NAMESPACE::w::input_field_config{},

       // This callback gets invoked by the "Ok" button, or when <Enter>
       // is pressed in the input field. In either case the dialog is hidden.
       //
       // This callback received the input_field widgets, so that
       // the callback may obtain the entered text.
       []
       (ONLY IN_THREAD,
        const INSERT_LIBX_NAMESPACE::w::input_dialog_ok_args &info)
       {
           INSERT_LIBX_NAMESPACE::w::input_lock lock{info.dialog_input_field};

           std::cout << "Your name is: " << lock.get() << std::endl;
       },

       // This callback gets invoked when the dialog gets hidden for other
       // reasons: the cancel button, or the dialog's close button, if
       // provided by the window manager.

       []
       (ONLY IN_THREAD,
        const INSERT_LIBX_NAMESPACE::w::ok_cancel_callback_args &args)
       {
           std::cout << "How rude..." << std::endl;
       });
\endcode

create_input_dialog() constructs an
dialog with a \ref input_field "input field", returning an
\ref input_dialog "INSERT_LIBX_NAMESPACE::w::input_dialog".

\code
main_window->stop_message("Something went wrong");

try {
    // ...
} catch (const INSERT_LIBX_NAMESPACE &e)
{
    main_window->exception_message(e);
}
\endcode

stop_message() is a shortcut that uses create_ok_dialog() to format and
show() a text label in a modal (by default) dialog using the "stop" icon.

exception_message() does the same with a thrown
<a href="/exceptions.html">exception</a> object.

An optional
\ref stop_message_config "INSERT_LIBX_NAMESPACE::w::stop_message_config"
parameter customizes the properties of the error dialog.

alert_message() creates a similar dialog that uses the "alert" icon,
and the optional
\ref alert_message_config "INSERT_LIBX_NAMESPACE::w::alert_message_config"
parameter.

\note
stop_message(), exception_message(), and alert_message()
do not wait until the dialog gets
closed, they return immediately. Use
\ref stop_message_config "INSERT_LIBX_NAMESPACE::w::stop_message_config"'s
or
\ref alert_message_config "INSERT_LIBX_NAMESPACE::w::alert_message_config"'s
\c acknowledged_callback to execute a callback when the dialog gets closed.

\code
#include <INSERT_LIBX_NAMESPACE/w/file_dialog.H>
#include <INSERT_LIBX_NAMESPACE/w/file_dialog_config.H>

INSERT_LIBX_NAMESPACE::w::file_dialog_config config{
    []
    (ONLY IN_THREAD,
     const INSERT_LIBX_NAMESPACE::w::file_dialog &d,
     const std::string &name,
     const INSERT_LIBX_NAMESPACE::w::busy &busy_mcguffin)
     {
          d->dialog_window->hide();

          // ...
     },
     [](ONLY IN_THREAD,
        const INSERT_LIBX_NAMESPACE::w::ok_cancel_callback_args &args)
     {

     }};

INSERT_LIBX_NAMESPACE::w::file_dialog d=main_window->create_file_dialog({"file_open@example.com", true}, config);
\endcode

create_file_dialog() returns a
\ref file_dialog "INSERT_LIBX_NAMESPACE::w::file_dialog", for selecting
an existing or a new file.

\code
#include <INSERT_LIBX_NAMESPACE/w/print_dialog.H>
#include <INSERT_LIBX_NAMESPACE/w/print_dialog_config.H>

INSERT_LIBX_NAMESPACE::w::print_dialog_config config;

INSERT_LIBX_NAMESPACE::w::print_dialog d=main_window->create_print_dialog({"print@example.com", true}, config);

\endcode

create_print_dialog() returns a
\ref print_dialog "INSERT_LIBX_NAMESPACE::w::print_dialog", for selecting
a printer and options for printing a document.

\par Managing existing dialogs.

\code
std::unordered_set<std::string> labels=main_window->dialogs();

INSERT_LIBX_NAMESPACE::w::dialogptr d=main_window->dialog("question@example.com");

main_window->remove_dialog("question@example.com");
\endcode

dialogs() returns the labels of the dialogs that currently exist.
dialog() returns a dialog with the specified label. A null \c ptr gets
returned if the dialog does not exist.

remove_dialog() removes the dialog from the main_window.

*/

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
