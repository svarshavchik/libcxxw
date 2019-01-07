/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_elementfwd_h
#define x_w_elementfwd_h

#include <x/w/namespace.H>
#include <x/w/callback_triggerfwd.H>
#include <x/w/connection_threadfwd.H>
#include <x/w/metrics/axisfwd.H>
#include <x/w/button_eventfwd.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

typedef void metrics_update_callback_t(THREAD_CALLBACK,
				       const metrics::axis &,
				       const metrics::axis &);

class LIBCXX_PUBLIC elementObj;

//! Cut or copy option

//! This gets passed to passed to \ref generic_window "cut_or_copy_selection()"
//! or \ref focusable "focusable_cut_or_copy_selection()".

enum class cut_or_copy_op {
			   available,  //!< Something can be cut or copied
			   copy,      //!< Copy something
			   cut,	      //!< Cut something
};

/*! Methods common to all display elements.

\code
e->appdata=INSERT_LIBX_NAMESPACE::mcguffinstash<std::string>::create();

INSERT_LIBX_NAMESPACE::w::screen screen=e->get_screen();

e->show();

e->show_all();

e->hide();

e->hide_all();

\endcode

\c appdata is an opaque object pointer for the application's use.
The library does not use it in any way.

get_screen() returns the element's \ref screen "screen object".

show() and hide() displays or hides the display element. show_all() and
hide_all() also shows or hides any other display elements contained
in this one (this display element is a \ref container "container").

\code
e->ensure_entire_visibility();

e->ensure_entire_visibility(IN_THREAD);
\endcode

Scrolls this display element into view if it's currently hidden inside
a \ref peepholelayoutmanager "peephole".

\code
auto p=e->set_background_color(INSERT_LIBX_NAMESPACE::w::rgb(INSERT_LIBX_NAMESPACE::w::rgb::maximum(), 0, 0));

e->set_background_color("action_color1");

e->remove_background_color();
\endcode

set_background_color() sets this element's background to an
\ref rgb "INSERT_LIBX_NAMESPACE::rgb"-specified color, or to a theme
color of the given name. remove_background_color() removes the element's
existing background color, reverting it to the default one.

\code
INSERT_LIBX_NAMESPACE::ref<INSERT_LIBX_NAMESPACE::obj> mcguffin=element->get_shade_busy_mcguffin();

INSERT_LIBX_NAMESPACE::ref<INSERT_LIBX_NAMESPACE::obj> mcguffin=element->get_wait_busy_mcguffin();

INSERT_LIBX_NAMESPACE::ref<INSERT_LIBX_NAMESPACE::obj> mcguffin=element->connection_mcguffin();
\endcode

get_shade_busy_mcguffin() and get_wait_busy_mcguffin()
return a \ref busy "mcguffin that blocks processing" of all
keyboard
or pointer button press events, until the last reference to the mcguffin
goes out of scope and it gets destroyed.

get_shade_busy_mcguffin() is equivalent to get_wait_busy_mcguffin() on
display screens that don't support alpha compositing.

connection_mcguffin() returns a mcguffin for the underlying connection
to the display server. Installing a destructor callback on this mcguffin
results in the callback getting invoked when the connection to the
underlying display server is closed and the internal execution thread,
for the connection, stops.

\par Pointer focus callback

\code
element->on_pointer_focus([]
                          (ONLY IN_THREAD,
                           INSERT_LIBX_NAMESPACE::w::focus_change f,
                           const INSERT_LIBX_NAMESPACE::w::callback_trigger_t &trigger)
                          {
                              // ...
                          });
\endcode

Installs a callback that reports the element's pointer focus state
(when the pointer moves in and out of the display element).

The callback also gets invoked immediately to report the element's
current focus state, after the callback gets installed.

The callback receives a \ref focus_change "focus_change" parameter
that indicates the new status of pointer focusability. The
\ref callback_trigger_t "trigger parameter" is mostly unused with
pointer callbacks, it's provided mostly for consistency with
a focusable element's on_keyboard_focus() callbacks. With pointer
callbacks the trigger has only one possible value: initial, indicating
the initial callback upon installation, and the default monostate
value in most other instances, which are result of pointer movement.

\note
Well, not immediately. The callbacks get invoked and updated by an
internal execution thread, and the callback's initial invocation will
likely occur after on_pointer_focus() returns.

Only one callback may get installed at a time. Installing a callback
replaces the previous callback. Since the callbacks are handled by
the aforementioned internal execution thread, it is possible that
the previous callback still gets invoked after on_pointer_focus()
returns, if the execution thread was about to call it. In any case,
after the execution thread invokes the replacement callback once, the
previous one is a distant memory.

\note
The callback cannot capture references to its display element, any of its
parent display elements, or any of its child display elements (if the
element is a container). This creates a circular references. Weak captures
must be used.

\par Button event callback

\code
#include <INSERT_LIBX_NAMESPACE/w/button_event.H>

element->on_button_event([]
                        (ONLY IN_THREAD,
                         const INSERT_LIBX_NAMESPACE::w::button_event &e,
                         bool activated,
                         const INSERT_LIBX_NAMESPACE::w::busy &mcguffin)
                         {
                             if (activated && e.button == 1)
                                 // ...
                         });
\endcode

Installs a callback that gets invoked whenever a pointer button gets
pressed or released with the pointer inside the element.

The callback gets invoked only if no other action occurs in response to the
button event.

\note
The callback cannot capture references to its display element, any of its
parent display elements, or any of its child display elements (if the
element is a container). This creates a circular references. Weak captures
must be used.

\par Metrics callback.

\code
#include <x/w/metrics/axis.H>

element->on_metrics_callback([]
                            (ONLY IN_THREAD,
                             const INSERT_LIBX_NAMESPACE::metrics::axis &h,
                             const INSERT_LIBX_NAMESPACE::metrics::axis &v)
                             {
                               // ...
                             });
\endcode

on_metrics_callback() installs a callback that gets invoked with the element's
current \ref metrics::axis "metrics". The callback gets invoked with the
element's current metrics, and then every time the metrics change. Only
one metrics callback can be installed at the same time, installing another
callback replaces the existing one.

\par Element state callback

\code
#include <x/w/element_state.H>

m->on_state_update([]
                   (ONLY IN_THREAD,
                    const INSERT_LIBX_NAMESPACE::w::element_state &s,
                    const INSERT_LIBX_NAMESPACE::w::busy &mcguffin)
                    {
                    });

m->on_state_update(ONLY IN_THREAD,
                   []
                   (ONLY IN_THREAD,
                    const INSERT_LIBX_NAMESPACE::w::element_state &s,
                    const INSERT_LIBX_NAMESPACE::w::busy &mcguffin)
                    {
                    });
\endcode

The state update callback reports changes to the display element's visibility,
and receives the \ref element_state "INSERT_LIBX_NAMESPACE::w::element_state".

\par Context popups

\code
#include <INSERT_LIBX_NAMESPACE/w/container.H>
#include <INSERT_LIBX_NAMESPACE/w/listlayoutmanager.H>

INSERT_LIBX_NAMESPACE::w::container context_popup=
    element->create_popup_menu([]
                               (const INSERT_LIBX_NAMESPACE::w::listlayoutmanager &lm)
                               {
                                      // ...
                               });


element->install_contextpopup_callback([context_popup]
                                      (ONLY IN_THREAD,
				       const INSERT_LIBX_NAMESPACE::w::element &e,
                                       const INSERT_LIBX_NAMESPACE::w::callback_trigger_t &trigger,
                                       const INSERT_LIBX_NAMESPACE::w::busy &mcguffin)
                                       {
                                              context_popup->show();
                                       },
                                       {"F3"});
                              });
element->remove_contextpopup();
\endcode

Each display element has an optional "context popup". An element's context
popup gets opened by right clicking on it with the right pointer button.
An optional keyboard shortcut also makes the popup visible if its display
element is visible.

create_popup_menu() creates a context popup as a \ref menubarfactory "menu".
It's parameter is a creator that gets invoked with a
\ref listlayoutmanager "list layout manager", and is expected to create
the popup menu's items. create_popup_menu() returns a \ref container "container"
object representing the popup menu.

The popup menu always appears next to the pointer. The expected way
to make the popup menu appear is to install a context popup callback.

install_contextpopup_callback()'s parameter is a callback that gets
in response the right pointer button click. The expected behavior is to
simply show() the context popup container, which makes it visible next
to the pointer's currnet position. install_contextpopup_callback()'s
optional second parameter is a \ref shortcut "keyboard shortcut" that also
invokes the callback.

The callback's first parameter is the element it's attached to. Its remaining
parameters are:

- what \ref callback_trigger_t "triggered" the callback.

- the \ref busy "busy mcguffin factory".

remove_context_popup() removes the element's current context popup callback.
Each
display element can only have one context popup installed, at a time.
Installing a new context popup callback automatically removes the previously
installed context popup callback; and remove_context_popup() explicitly
removes the currently installed one.

\note
The display element owns a reference on its installed context popup callback.
The display element does not directly own a reference on any of its context
popup callbacks, and the popups do not own a reference on its display element.
The above example captures the context popup container, for the callback.
This results in the display element owning a reference on its callback,
and the callback owning a reference on the popup, so it remains in scope
until the callback gets removed.

\note
Only containers created using the element's create_popup_menu() should be
shown by the installed callback. Using a container created by another element's
create_popup_menu() has unspecified results.

\par Invoke an arbitrary callback in the display element's connection thread.

\code
conn->in_thread([]
                (ONLY IN_THREAD)
                {
                    // ...
                });

conn->in_thread_idle([]
                    (ONLY IN_THREAD)
                    {
                       // ...
                    });
\endcode

in_thread() returns immediately, and executes the passed-in callable
object by the internal connection thread that's responsible for updating
the visual appearance of this display element. The callback gets executed
after repositioning and resizing any display elements (not just this one)
due to any pending metric and size changes, but before processing X
display server messages or updating the display.

in_thread_idle() returns immediately, and executes the passed-in callable
object by the internal connection thread. The callback gets executed
after all other activity gets processed; after processing any X
display server messages, and updating the display with any
unprocessed changes.


*/

typedef ref<elementObj> element;

//! A nullable pointer reference to an element.

typedef ptr<elementObj> elementptr;

//! A reference to a constant element.

//! \see element

typedef const_ref<elementObj> const_element;

//! A nullable pointer reference to a constant element.

//! \see element

typedef const_ptr<elementObj> const_elementptr;

//! Signature of a context menu callback installer.

typedef void install_contextpopup_callback_t(THREAD_CALLBACK,
					     const element &,
					     const callback_trigger_t &,
					     const busy &);
//! Signature of a button event callback

//! The callback returns true if it processed the button event.

//! The first parameter is the button event.
//!
//! The second parameter indicates whether the button press or release is
//! considered to be "activation" of that button.
//!
//! Generally, if it's the same as button_event.press, then do something.

typedef bool button_event_callback_t(THREAD_CALLBACK,
				     const button_event &,
				     bool,
				     const busy &);

LIBCXXW_NAMESPACE_END

#endif
