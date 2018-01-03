/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_panefactoryfwd_h
#define x_w_panefactoryfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class panefactoryObj;

/*! A factory for new panes in a \ref panelayoutmanager "pane layout manager"'s container.

\code
f->create_label("Hello world!")->show();
\endcode

New elements get created as individual panes with adjustable sizes.

\code
f->set_initial_size(10.0).padding(2).create_label("Hello world!")->show();
\endcode

set_initial_size() sets the initial size of the next pane.
Pane sizes get specified in
\ref dim_arg "millimeters or theme-specified dimensions".
Rather than
sizing the pane container from the sizes of all individual elements, the
overall size of a pane container is the sum total of all panes' specified
initial sizes, plus the sizes of the divider/sliders, plus the inner padding
of each pane.

Initially each pane shows only as much of the pane's element as fits within
the specified pane initial size. Sliding dividers get placed between panes.
The mouse or keyboard to moves the sliders. This adjust how the pane
container overall space gets divided between all the panes.

\note
Each new pane element's needs to get show()n to be visible.

\par Miscellaneous pane properties

Use these \c panefactory methods before creating a new pane element.
This specifies the new pane's custom properties.

\code
f->set_initial_size(10.0)
  .set_background_color(INSERT_LIBX_NAMESPACE::w::rgb{0, 0, INSERT_LIBX_NAMESPACE::w::rgb::maximum()})
  .halign(INSERT_LIBX_NAMESPACE::w::halign::centered)
  .valign(INSERT_LIBX_NAMESPACE::w::valign::middle);
  .set_scrollbar_visibility(INSERT_LIBX_NAMESPACE::w::scrollbar_visibility::always)
  .create_label("Hello world")->show();

\endcode

set_background_color() sets a custom background color for the next pane.

The individual panes may be expanded beyond their elements' initial size.
halign() and valign() sets the position alignment of the next pane's
element when its smaller than its pane.

set_scrollbar_visibility() controls the visibility of the new pane's
scroll-bar. Each pane's scroll-bar provides an alternative to using the
nearby sliders to make the pane bigger. The default
\ref scrollbar_visibility "scrollbar visibility"
is \c INSERT_LIBX_NAMESPACE::w::scrollbar_visibility::automatic_reserved.

\note
Using the factory to create multiple display elements places each individual
element into a new pane. The custom properties are not persistent.
For example, set_background_color() sets only the next new display element's
pane's background color. Call set_background_color() again to use the same
(or different) background color before creating the netx display element.

*/

typedef ref<panefactoryObj> panefactory;

//! A constant \ref panefactory "pane factory".

//! \see panefactory

typedef const_ref<panefactoryObj> const_panefactory;

//! A nullable pointer reference to a \ref panefactory "pane factory".

//! \see panefactory

typedef ptr<panefactoryObj> panefactoryptr;

//! A nullable pointer reference to a const \ref panefactory "pane factory".

//! \see panefactory

typedef const_ptr<panefactoryObj> const_panefactoryptr;

LIBCXXW_NAMESPACE_END

#endif
