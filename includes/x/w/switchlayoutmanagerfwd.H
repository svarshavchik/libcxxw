/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_switchlayoutmanagerfwd_h
#define x_w_switchlayoutmanagerfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>
#include <x/mpobj.H>

LIBCXXW_NAMESPACE_START

class switchlayoutmanagerObj;

/*! The switch layout manager.

\code
#include <INSERT_LIBX_NAMESPACE/w/switchlayoutmanager.H>

factory->create_container([]
                          (const auto &new_container)
                          {
                              INSERT_LIBX_NAMESPACE::w::switchlayoutmanager layout_manager=new_container->get_layoutmanager();

                              // ...
                          },
                          INSERT_LIBX_NAMESPACE::w::new_switchlayoutmanager{});

\endcode

This layout manager switches between showing one of the elements in its
container, at a time. Only one of the elements in the switched container
appears, at a time. Switching to one of the elements makes the currently
visible element disappear.

\code
#include <INSERT_LIBX_NAMESPACE/w//switchfactory.H>

INSERT_LIBX_NAMESPACE::w::switchfactory factory=layout_manager->append();

auto label=factory->create_label("Hello world")->show();

INSERT_LIBX_NAMESPACE::w::switchfactory factory=layout_manager->insert(0);

layout_manager->remove(1);

\endcode

The list of individual display elements in the switched container
is numbered, starting
with element #0. append() returns a \ref switchfactory "factory" that
adds new elements to the end of the list; insert() returns a factory
that inserts new elements between an existing element in the list,
automatically renumbering them; and remove() removes one of them.

\note
Each display element in the switched container must still be show()n
in order to be visible when its turn comes to shine, even though
the switch layout manager only makes one of them visible, at a time.

lookup() returns the current index of the given element.

\code
std::optional<size_t> layout_manager->lookup(label);

layout_manager->switch_to(layout_manager->size()-1);

layout_manager->switch_off();

std::optional<size_t> n=layout_manager->switched();

INSERT_LIBX_NAMESPACE::w::element e=layout_manager->get(0);

\endcode

size() returns the number of elements in the switched container.
lookup() returns the index of the given element. lookup() returns nothing
if the given element is not in the container: you looked up some other
element.

switch_to() makes the given element number (the elements are numbered starting
with 0) visible, and makes the currently-visible element, if there is one,
disappear.

switch_off() makes the currently visible element disappear,
without making any other one appear. switched() indicates which element
in the switch is visible.

get() returns element \#n.

\code
INSERT_LIBX_NAMESPACE::w::switch_lock lock{layout_manager};
\endcode

Constructing the switch lock object blocks all other execution threads
from accessing the switch layout manager. The number of elements in the
container, and other aspects of the container, may be examined using
the switch layout manager as a stable, consistent snapshot, blocking
other execution threads from modifying the switched container.

\note
This also includes the library's internal execution thread. An existing
switch lock can block the internal execution thread from updating the
display.

*/

typedef ref<switchlayoutmanagerObj> switchlayoutmanager;

//! A constant \ref switchlayoutmanager "switch layout manager".

//! \see switchlayoutmanager

typedef const_ref<switchlayoutmanagerObj> const_switchlayoutmanager;

//! A nullable pointer reference to a \ref switchlayoutmanager "switch layout manager".

//! \see switchlayoutmanager

typedef ptr<switchlayoutmanagerObj> switchlayoutmanagerptr;

//! A nullable pointer reference to a const \ref switchlayoutmanager "switch layout manager".

//! \see switchlayoutmanager

typedef const_ptr<switchlayoutmanagerObj> const_switchlayoutmanagerptr;

struct LIBCXX_HIDDEN switch_layout_info_s;

//! Internal switch layout manager mutex-protected info.

//! \internal
typedef mpobj<switch_layout_info_s, std::recursive_mutex> switch_layout_info_t;


LIBCXXW_NAMESPACE_END

#endif
