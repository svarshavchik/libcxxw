/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_panefactoryfwd_h
#define x_w_panefactoryfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC panefactoryObj;

/*! A factory for new panes in a \ref panelayoutmanager "pane layout manager"'s container.

\code
f->create_label("Hello world!")->show();
\endcode

New elements get created as individual panes with adjustable sizes.

\code
#include <INSERT_LIBX_NAMESPACE/w/pane_appearance.H>

f->appearance=f->appearance->modify([]
                                    (const INSERT_LIBX_NAMESPACE::w::pane_appearance &custom)
                                    {
                                        custom->size=10.0;
                                        custom->left_padding=
                                            custom->right_padding=
                                            custom->bottom_padding=2;
                                    });


f->create_label("Hello world!")->show();
\endcode

The \c appearance member specifies the appearance of the next new pane.
\c appearance is a reference to a constant object. The default appearance
comes from the current display theme. A custom appearance for
the next new pane gets created by using an existing appearance object's
modify(). This invokes the passed in closure that receives a modifiable
copy of the appearance object. modify() returns the new constant appearance
object.

The panes maintain a reference on their appearance object, and the connection
thread may access the panes' appearance objects at any time, and this
convention ensures, by contract, that the appearance objects are constant and
thread safe, and the only time the appearance objects exist is when they
get created as a copy of an original constant appearance object.

Each time a new pane gets create, the \c appearance member gets reset back
to the default theme-specified appearance.

Pane sizes get specified in
\ref dim_arg "millimeters or theme-specified dimensions".
Rather than
sizing the pane container from the sizes of all individual elements, the
overall size of a pane container is the sum total of all panes' specified
initial sizes, plus the sizes of the divider/sliders, plus the inner padding
of each pane.

Initially each pane shows only as much of the pane's element as fits within
the specified pane initial size. Sliding dividers get placed between panes.
The mouse or keyboard moves the sliders. This adjusts how the pane
container overall space gets divided between all of its panes.

\note
Each new pane's element needs to get show()n to be visible.

\par Miscellaneous pane appearance properties.

\code
f->appearance=f->appearance->modify([]
                                    (const INSERT_LIBX_NAMESPACE::w::pane_appearance &custom)
                                    {
                                        custom->size=10.0;
                                        custom->left_padding=
                                            custom->right_padding=
                                            custom->bottom_padding=2;
                                        custom->background_color=INSERT_LIBX_NAMESPACE::w::blue;
                                        custom->horizontal_alignment=INSERT_LIBX_NAMESPACE::w::halign::centered;
                                        custom->vertical_alignment=INSERT_LIBX_NAMESPACE::w::valign::middle;
                                        custom->pane_scrollbar_visibility=INSERT_LIBX_NAMESPACE::w::scrollbar_visibility::always;

                                    });

\endcode

\c background_color sets the new pane's custom background color.

The individual panes may be expanded beyond their elements' initial size.
\c horizontal_alignment and \c vertical_alignment
sets the position alignment of the next pane's
element when its smaller than its pane.

\c pane_scrollbar_visibility controls the visibility of the new pane's
scroll-bar. Each pane's scroll-bar provides an alternative to using the
nearby sliders to make the pane bigger. The default
\ref scrollbar_visibility "scrollbar visibility"
is \c INSERT_LIBX_NAMESPACE::w::scrollbar_visibility::automatic_reserved.

\note
Using the factory to create multiple widgets places each individual
element into a new pane, and resets the pane appearance to the default
theme-specified appearance. Creating multiple panes with a custom background
color, for example, requires taking the pane appearance object from the
factory, using modify(), and setting its background_color to the same
pane's background color. Call set_background_color() again to use the same
(or different) background color before creating the next widget.

\par Panes with selection lists and tables

\code
INSERT_LIBX_NAMESPACE::w::new_listlayoutmanager nlm;

pane_f->configure_new_list(nlm);

pane_f->appearance=pane_f->appearance->modify([]
                                              (const auto &custom)
                                              {
                                                  custom->size=20;
                                              });

pane_f->create_focusable_container(
         []
         (const auto &container)
         {
             INSERT_LIBX_NAMESPACE::w::listlayoutmanager lm=container->get_layoutmanager();

             lm->append_items({
                     "Lorem ipsum",
                     "dolor sit amet",
                     "consectetur",
                     "adipisicing elit sed",
                     "do eiusmod",
                     "tempor incididunt ut",
                     "labore et",
                     "dolore magna",
                     "aliqua"});
         },
    	 nlm)->show();
\endcode

configure_new_list() gets called
before calling create_focusable_container() for a table or a list.
Passing a
\ref new_listlayoutmanager "INSERT_LIBX_NAMESPACE::w::new_listlayoutmanager"
or
\ref new_tablelayoutmanager "INSERT_LIBX_NAMESPACE::w::new_tablelayoutmanager"
to configure_new_list() installs a modified pane appearance object that
adjusts several settings for an intuitive interface
consisting of a vertical pane with a \ref listlayoutmanager "selection list"
or a
\ref tablelayoutmanager "table."

Ordinary selection lists have a fixed height specified as the number of
rows in the list, and the selection list provides a scroll-bar for longer
lists. Tables also have a fixed height by default.

configure_new_list() installs a new appearance object with
adjusted padding and position, settings
and disables the scrollbar; and adjusts the new table or list to have
an adjustable height, and no borders. The pane layout manager-provided borders
are sufficient.

Setting configure_new_list()'s optional second parameter to \c true
indicates that the new list is synchronized with another list; most likely
the list in the preceding or the following pane. It's necessary to make those
lists' scrollbars \c always visible; so all synchronized lists always have
the same base width, preventing the situation where one list is wider
than the others because it does not currently show a scrollbar (the
\c automatic_reserved setting does not result in good visual appearance).
This will prevent the lists' column widths from getting correctly synchronized.

After this selection list gets placed in the pane,
the pane's sliders indirectly adjust the selection list's height. The
pane's sliders adjust the pane's height, and with the pane's now having
a variable height, the height of the selection list is always the same as
its containing pane.

The end result is a selection list or a table
 that looks like an integral part of the pane container.
This result gets further improved by automatically incorporating
the \ref focusable "selection list's tabbing order" into
the pane's tabbing order. Normally the pane's sliders'
and its scroll-bars' tabbing order is independent of any focusable
elements in the container's pane.

When a focusable container gets created as a pane (a selection list
is a focusable container), the focusable container's tabbing order
gets inserted in the natural order of the pane container's individual tabbable
elements -- the sliders and the pane's own scroll-bars (from other panes
in the container). Do not directly adjust the individual tabbing order of
focusable containers in the pane, the pane handles it automatically.

This special treatment gets afforded only to focusable containers that
get created as individual panes, and does not affect any other focusable
elements, that get created either directly as panes, or as individual
elements of non-focusable container panes. Those focusable elements continue
to have independently-adjustable tabbing order.

\note
configure_new_list() installs a new, default appearance object. Any
custom settings for the new pane must be done after configure_new_list().

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
