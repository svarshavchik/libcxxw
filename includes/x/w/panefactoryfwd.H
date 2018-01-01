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

New elements get created in individual panes.

\code
f->set_initial_size(10.0)
  .set_background_color(INSERT_LIBX_NAMESPACE::w::rgb{0, 0, INSERT_LIBX_NAMESPACE::w::rgb::maximum()})
  .padding(2);
\endcode

set_initial_size() sets the initial size of the next pane.
Pane sizes get specified in
\ref dim_arg "millimeters or theme-specified dimensions".
Rather than
sizing the pane container from the sizes of all individual elements, the
overall size of a pane container is the sum total of all panes' initial
sizes, plus the sizes of the divider/sliders, plus the inner padding of
each pane.

set_background_color() sets a custom background color of the next pane.

\code
f->halign(LIBCXX_NAMESPACE::w::halign::centered);

f->valign(LIBCXX_NAMESPACE::w::valign::middle);
\endcode

The individual panes may be expanded beyond their elements' initial size.
halign() and valign() sets the position alignment of the next pane's
element if its smaller than its pane.

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
