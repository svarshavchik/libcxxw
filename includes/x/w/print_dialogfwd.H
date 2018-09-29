/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_print_dialogfwd_h
#define x_w_print_dialogfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC print_dialogObj;

struct print_dialog_args;

/*! Select a printer and print options.

\code
#include <INSERT_LIBX_NAMESPACE/w/print_dialog.H>
#include <INSERT_LIBX_NAMESPACE/w/print_dialog_config.H>
#include <INSERT_LIBX_NAMESPACE/cups/job.H>

INSERT_LIBX_NAMESPACE::w::print_dialog_config config;

config.print_callback=[]
		      (const INSERT_LIBX_NAMESPACE::w::print_callback_info &info)
                      {
                           INSERT_LIBX_NAMESPACE::cups::job job=info.job;
			   INSERT_LIBX_NAMESPACE::ref<INSERT_LIBX_NAMESPACE::obj> wait_mcguffin=info.wait_mcguffin;
                           // ...
                      };

config.cancel_callback=[]
                       {
                           // ...
                       };

INSERT_LIBX_NAMESPACE::w::print_dialog pd=main_window->create_print_dialog("print_dialog", config, true);

// ...

pd->initial_show();

// ...
\endcode

\ref main_window "INSERT_LIBX_NAMESPACE::w::main_window"'s
create_print_dialog() method returns a \c print_dialog.

This dialog lists available printers,
and available print options. The first parameter is a unique dialog
identifier, like with other \c main_window dialog factories.
The second parameter is a
\ref print_dialog_config "INSERT_LIBX_NAMESPACE::w::print_dialog_config",
and third parameter specified whether the print dialog is modal, or not.

\par Dialog configuration

\c INSERT_LIBX_NAMESPACE::w::print_dialog_config specifies the following
dialog options.

- print_callback: this callback gets executed by the print dialog's
"Print" button.
The "Print" button closes its dialog before invoking the callback, passing
it a \ref print_callback_info "INSERT_LIBX_NAMESPACE::w::print_callback_info"
parameter. The parameter's shining star is a
<a href="/cups_job.html">INSERT_LIBX_NAMESPACE::cups::job</a> object
with the selected print options already set. The co-star is the
opaque \ref busy "wait mcguffin" which blocks pointer and keyboard
press events for print dialog's parent main window. Printing a document is
typically a time consuming process, so the wait mcguffin gets
created and passed as a parameter to this callback.

- cancel_callback: this callback gets executed if the print dialog gets
closed by its "Cancel" button, or if it gets closed by the window
manager.

\par Opening the print dialog

Do not use the dialog window's \c show_all() method to show the print
dialog. \c create_print_dialog() creates an empty, uninitialized
print dialog. \c initial_show() returns immediately after starting
a background execution thread that connects to the print spooler and
initializes the print dialog, then shows it.

initial_show() takes care of acquiring a wait mcguffin for the
main window, until the print dialog becomes available. As such,
initial_show() appears to a single, fluid action that puts up a "busy"
pointer in the main window until the print dialog opens.

\par Printing a document

The
<a href="/cups_job.html">INSERT_LIBX_NAMESPACE::cups::job</a> object
the print callback receives already have all the print options set,
from the dialog. The only thing that needs to be done is to add the document
contents to the object and invoke its submit() method. Because preparing
the printed document should be a time-consuming task, the callback
also gets the wait mcguffin. The callback does not need to start a new
execution thread and pass the wait mcguffin to it. The \c print_callback itself
gets invoked from a new execution thread, and the library's internal execution
thread that handles display events continues executing concurrently (in
addition to the application's main execution thread).

*/

typedef ref<print_dialogObj> print_dialog;

//! A constant \ref print_dialog "print dialog".

//! \see print_dialog

typedef const_ref<print_dialogObj> const_print_dialog;

//! A nullable pointer reference to a \ref print_dialog "print dialog".

//! \see print_dialog

typedef ptr<print_dialogObj> print_dialogptr;

//! A nullable pointer reference to a const \ref print_dialog "print dialog".

//! \see print_dialog

typedef const_ptr<print_dialogObj> const_print_dialogptr;

LIBCXXW_NAMESPACE_END

#endif
