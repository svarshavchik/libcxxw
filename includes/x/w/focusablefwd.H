/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_focusablefwd_h
#define x_w_focusablefwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC focusableObj;

/*! Methods common to all widget that can receive keyboard focus.

A widget with keyboard input focus receives and processes all
keyboard input. All widgets capable of processing keyboard input
are kept in an internal list. TAB and BACK-TAB transfers keyboard focus
to the next or the previous element in the list.

\code
f->set_enabled(false);
\endcode

\c set_enabled(false) blocks this widget from receiving keyboard
focus. The keyboard focus gets moved to another widget if this
element has keyboard focus at the time it is disabled.

\c set_enabled(true) makes this focusable element enabled for processing
keyboard and button press events.

Disabled focusable elements are drawn at half intensity. \c set_enabled()
redraws the widgets accordingly.

\code
f->get_focus_after(other);

f->get_focus_before(other);

f->get_focus_first();

f->request_focus();

f->request_focus(true);

f->get_focus_after_me({other1, other2, other3});

f->get_focus_before_me({other1, other2, other3});


f->get_focus_after(IN_THREAD, other);

f->get_focus_before(IN_THREAD, other);

f->get_focus_first(IN_THREAD);

f->request_focus(IN_THREAD);

f->request_focus(IN_THREAD, true);

f->get_focus_after_me(IN_THREAD, {other1, other2, other3});

f->get_focus_before_me(IN_THREAD, {other1, other2, other3});


\endcode

The parameter to \c get_focus_after() and \c get_focus_before() is another
focusable widget in the same window. These functions move
the tabbing order of this widget.

get_focus_after() sets this focusable element's tabbing order to be after
the other widget's. When the other focusable has keyboard
focus, TAB moves the focus to this focusable, then whichever other
element originally followed the other element in the tabbing order. In
other words, other's tabbing order with respect to all other elements does
not change, except that this focusable now gets tabbed to after this one,
instead of its original tabbing order.

get_focus_before() sets this focusable element's tabbing order to be before
the other widget's. The other focusable's tabbing order with
respect
to all other elements does not change, except that a TAB from the previous
focusable in the tabbing order first goes to this focusable, than the other
one.

get_focus_first() moves this focusable element's tabbing order to be the
first tabbable widget in its window.

get_focus_after_me() and get_focus_before_me() efficiently rearranges the
tabbing order of multiple widgets. The second parameter is a
\c std::vector of focusable objects. These methods do not change this
focusable's tabbing order with respect to other focusables. They change
the tabbing order of the focusables that get passed in the vector.

get_focus_after_me() sets their tabbing order to follow this element's
position in the tabbing order, in order of appearance in the vector.
get_focus_before_me() sets the tabbing order to precede this element's
position in the tabbing order (TAB-ing from the last element in the vector
lands on this focusable).

The behavior of having duplicate focusables in the vector, or having this
focusable in the vector too, is unspecified.

request_focus() moves the current keyboard focus to this focusable
element, if it's enabled and visible. An optional bool parameter of true
indicates that nothing should happen if the focusable element cannot
receive focus (it is disabled on not visible). By default, if the widget
cannot received keyboard focus it's presumed this is because it's just
been created, and all the moving pieces to make it visible and ready to
receive input focus did not happen yet, and the keyboard focus change
occurs as soon as the widget is visible and enabled. Note that this builds
a mousetrap if the widget is explicitly not focusable. The mousetrap gets
sprung when the widget gets enabled later (unless some other widget
requests keyboard focus first, explicitly or implicitly).

The IN_THREAD overloads can only be invoked in the connection thread.
Additionally, the IN_THREAD overloads return an indication if the focus was
successfully moved.

\code
#include <INSERT_LIBX_NAMESPACE/w/focus.H>

f->on_keyboard_focus([]
                     (ONLY IN_THREAD,
                      INSERT_LIBX_NAMESPACE::w::focus_change f,
                      const INSERT_LIBX_NAMESPACE::w::callback_trigger_t &trigger)
                     {
                        if (INSERT_LIBX_NAMESPACE::w::in_focus(f))
                             // ...
                        // ...
                     });

#include <INSERT_LIBX_NAMESPACE/w/key_event.H>
f->on_key_event([]
                     (ONLY IN_THREAD,
                      const INSERT_LIBX_NAMESPACE::w::all_key_events_t &e,
                      bool activated,
                      const busy &mcguffin)
                     {
                        // ...

                        return true;
                     });
\endcode

on_keyboard_focus() installs a callback that reports the widget's
focus change events. Immediately upon installation, the callback gets
invoked to report the element's most recent focus change event, and gets
invoke to report all future ones.

Only one focus callback can be installed. Installing another keyboard
focus change callback replaces any existing one.

The callback receives a \ref focus_change "focus_change" parameter
that indicates the new status of keyboard focusability. The
\ref callback_trigger_t "trigger parameter" indicates what event
triggered the focus change, which can range between: a button click
on the focusable element; \c prev_key and \c next_key indicate
focus navigation via the \c TAB key.

on_key_event() installs a callback that receives key events. There are
two possible key events: a
\ref key_event "key press and release event", or
text entered via an X Input Method server, that comes in as a
\c std::u32string_view. The parameter to a callback is a
\c std::variant of the two, defined as a
\ref all_key_events_t "INSERT_LIBX_NAMESPACE::w::all_key_events_t".
This callback gets invoked only when the widget
has keyboard focus. The
on_key_event() callback should return \c true if the key event
has been processed/consumed by the callback, and will not
result in any further processing. Returning \c false results in the
default processing for the respective key/text input.

The second \c bool parameter specifies whether this key event is
considered to be an "activation" of that key. Key event callbacks get
executed for both key press and release events, as specified in the
\ref key_event "INSERT_LIBX_NAMESPACE::w::key_event". This flag gets set
based on whether the key press, or the release, is considered to be
"activating", that is, whether a corresponding action associated with the
key should take places. This flag gets set either for a key press or the
release, depending on the widget. The flag is always set for
a pasted \c std::u32string_view.

\code
f->autofocus(true);
\endcode

autofocus() sets whether the widget is eligible for automatically
getting the keyboard focus when its main window gets shown. The first
widget with autofocus gets the keyboard focus.

The default autofocus() setting depends on the widget.

\par Selections

\code
bool flag=f->focusable_cut_or_copy_selection(INSERT_LIBX_NAMESPACE::w::cut_or_copy_op::available);

bool flag=f->focusable_cut_or_copy_selection(IN_THREAD, INSERT_LIBX_NAMESPACE::w::cut_or_copy_op::available);

f->focusable_cut_or_copy_selection(INSERT_LIBX_NAMESPACE::w::cut_or_copy_op::copy);

f->focusable_cut_or_copy_selection(IN_THREAD, INSERT_LIBX_NAMESPACE::w::cut_or_copy_op::copy);

f->focusable_receive_selection();

f->focusable_receive_selection(IN_THREAD)
\endcode

focusable_cut_or_copy_selection() or focusable_receive_selection()
is equivalent to invoking
cut_or_copy_selection() or receive_selection() on the
\ref generic_window "top level window object", except that the top level
window object executes the operation
for whichever element in the top level window currently has keyboard focus;
and focusable_cut_or_copy_selection() or focusable_receive_selection()
does this only if this focusable element
currently receives the keyboard focus, and does nothing otherwise.

*/

typedef ref<focusableObj> focusable;

//! A nullable pointer reference to a focusable widget.

typedef ptr<focusableObj> focusableptr;

//! A reference to a constant focusable widget.

//! \see focusable

typedef const_ref<focusableObj> const_focusable;

//! A nullable pointer reference to a constant focusable widget.

//! \see focusable

typedef const_ptr<focusableObj> const_focusableptr;

LIBCXXW_NAMESPACE_END

#endif
