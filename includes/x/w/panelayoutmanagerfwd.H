/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_panelayoutmanagerfwd_h
#define x_w_panelayoutmanagerfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC panelayoutmanagerObj;
class LIBCXX_HIDDEN new_panelayoutmanager;
struct LIBCXX_PUBLIC pane_style;

/*! Pane layout manager

A series of panes, stacked in a vertical column or a horizontal row in order
to use less real estate for displaying larger contents.

Each element in a container with a pane layout manager is placed into
a single column (vertical panes) or row (horizontal panes). The elements
are separated by thin dividers which slide, expanding the amount of the
overall container that's allocated to each element, at expense of the
other elements in the pane.

The width of vertical panes, and height of horizontal panes, gets
automatically sized to the widest (tallest) pane.

Each individual pane could be smaller than the pane's element, partially
(or even fully) hiding it. Move the dividing sliders on each side of the
element's pane to see more of the pane's element. Alternatively, use each
pane's scroll-bar to scroll different parts of the pane's element into view.

\code
#include <INSERT_LIBX_NAMESPACE/w/panelayoutmanager.H>

INSERT_LIBX_NAMESPACE::w::new_panelayoutmanager nplm;

nplm.slider_background_color="100%";

factory->create_focusable_container(
    [&]
    (const INSERT_LIBX_NAMESPACE::w::focusable_container &c)
    {
        INSERT_LIBX_NAMESPACE::w::panelayoutmanager pane_lm=c->get_layoutmanager();
    },
    nplm);
\endcode

Passing an \ref new_panelayoutmanager "INSERT_LIBX_NAMESPACE::w::new_panelayoutmanager"
to create_focusable_container() creates a new container that uses the
pane layout manager. new_panelayoutmanager's members set the new container's
custom borders and colors.

Only one execution thread has access to the underlying container, at a time,
using the pane layout manager, just like the
\ref gridlayoutmanager "grid layout manager", from which it derives.

\code
#include <INSERT_LIBX_NAMESPACE/w/panefactory.H>

size_t n=pane_lm->size();

INSERT_LIBX_NAMESPACE::w::element e=lm->get(0);

INSERT_LIBX_NAMESPACE::w::panefactory pane_f=pane_lm->append_panes();

INSERT_LIBX_NAMESPACE::w::panefactory pane_f=pane_lm->insert_panes(0);

INSERT_LIBX_NAMESPACE::w::panefactory pane_f=pane_lm->replace_panes(0);

pane_lm->remove_pane(1);

pane_lm->remove_all_panes();

INSERT_LIBX_NAMESPACE::w::panefactory pane_f=pane_lm->replace_all_panes();
\endcode

append_panes() returns a factory that adds new panes after the container's
existing panes. Each widget created with the returned factory's
methods gets placed into a new pane.

The pane layout manager automatically numbers the panes in the container,
starting with pane #0. size() returns the number of panes currently in
the container. get() returns the pane element.

Use
\ref panefactory "INSERT_LIBX_NAMESPACE::w::panefactory"'s methods before
creating a new element to set the new element's pane properties, such as
the pane's initial size, and non-default inner padding.

insert_panes()'s factory adds new panes before an existing pane in the
container.
remove_pane() removes an existing pane. remove_all_panes() removes all
panes from the container. replace_all_panes() is equivalent to using
remove_all_panes(), followed by append_panes().

\par Pane focusability.

\code
#include <INSERT_LIBX_NAMESPACE/w/panelayoutmanager.H>

INSERT_LIBX_NAMESPACE::w::new_panelayoutmanager nplm;

nplm.slider_background_color="100%";

LIBCXX_NAMESPACE::w::focusable_container c=factory->create_focusable_container(
    [&]
    (const INSERT_LIBX_NAMESPACE::w::focusable_container &c)
    {
        // ...
    },
    nplm);

c->get_focus_before(another_focusable);
\endcode

The pane layout manager's container is \ref focusable "focusable", whose
tabbing order is adjustable with respect to other focusable widgets.
The focusable elements in the pane container include the dividing sliders,
and each pane's scroll-bars, when they're visible.

In addition to adjusting the slider elements by dragging them with the
pointer, the slider elements can be tabbed to, using the keyboard,
and moved using the cursor keys. The tabbing order in the pane container
goes through the slider elements and all visible pane scroll-bars in order.

Each individual pane may have containers with individual focusable elements.
The tabbing order of the focusable elements is independent from the
pane container's tabbing order, they do not "interleave". For that reason
it's preferred to have their tabbing order immediately follow or precede
their pane container.

*/

typedef ref<panelayoutmanagerObj> panelayoutmanager;

//! A constant \ref panelayoutmanager "pane layout manager".

//! \see panelayoutmanager

typedef const_ref<panelayoutmanagerObj> const_panelayoutmanager;

//! A nullable pointer reference to a \ref panelayoutmanager "pane layout manager".

//! \see panelayoutmanager

typedef ptr<panelayoutmanagerObj> panelayoutmanagerptr;

//! A nullable pointer reference to a const \ref panelayoutmanager "pane layout manager".

//! \see panelayoutmanager

typedef const_ptr<panelayoutmanagerObj> const_panelayoutmanagerptr;

LIBCXXW_NAMESPACE_END

#endif
