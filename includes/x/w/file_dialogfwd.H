/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_file_dialogfwd_h
#define x_w_file_dialogfwd_h

#include <x/w/namespace.H>
#include <x/w/file_dialog_configfwd.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC file_dialogObj;

/*! \anchor file_dialog A file open/create dialog.

\code
INSERT_LIBX_NAMESPACE:w::main_window mw;

// ...

INSERT_LIBX_NAMESPACE::w::file_dialog_config c;

INSERT_LIBX_NAMESPACE::w::file_dialog fd=mw->create_file_dialog(c);

// ...

fd->the_dialog()->set_window_title("Pick a file");

fd->the_dialog()->show_all();

\endcode

This object gets returned by \ref main_window "create_file_dialog",
representing a simple dialog for browsing folders, and choosing a file to
create or open.

The complexity of the task at hand means that a plain \ref dialog "dialog"
object is not sufficient. The file dialog is represented by a discrete
\c file_dialog object.

the_dialog() returns the dialog widget, for directly invoking its
methods, such as set_window_title() and show_all(). create_file_dialog()
constructs the complete dialog and all of the individual widgets
that make it up; requiring a call to show_all() to make all of that visible.

\par Custom file dialog appearance.

\code
c.appearance=c.appearance->modify([]
                                  (const INSERT_LIBX_NAMESPACE::w::file_dialog_appearance &custom)
                                  {
                                      // ...
                                  });
\endcode

\ref file_dialog_config "INSERT_LIBX_NAMESPACE::w::file_dialog_config"'s
appearance member is a const
\ref file_dialog_appearance "INSERT_LIBX_NAMESPACE::w::file_dialog_appearance"
object (actually a \c const_file_dialog_appearance),
that contains many settings that control the visual appearance
of the file dialog (fonts and colors).

Customized file dialog properties are specified by using
modify() to create a copy of the appearance member, and invoke the passed-in
callable object or a closure that can access the temporarily
modifiable appearance object, and then replacing the original constant
appearance object.

This calling convention ensures thread-safety by contract: while running,
an existing file dialog safely accesses the constant appearance object that
created it, and any subsequent changes to the file dialog appearance always
use a new, modified appearance object.

*/

typedef ref<file_dialogObj> file_dialog;

//! A constant \ref file_dialog "file dialog".

//! \see file_dialog

typedef const_ref<file_dialogObj> const_file_dialog;

//! A nullable pointer reference to a \ref file_dialog "file dialog".

//! \see file_dialog

typedef ptr<file_dialogObj> file_dialogptr;

//! A nullable pointer reference to a const \ref file_dialog "file dialog".

//! \see file_dialog

typedef const_ptr<file_dialogObj> const_file_dialogptr;

LIBCXXW_NAMESPACE_END

#endif
