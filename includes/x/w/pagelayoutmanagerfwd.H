/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_pagelayoutmanagerfwd_h
#define x_w_pagelayoutmanagerfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>
#include <x/mpobj.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC page_lock;
class LIBCXX_PUBLIC pagelayoutmanagerObj;
class LIBCXX_HIDDEN new_pagelayoutmanager;

/*! The page layout manager.

\code
#include <INSERT_LIBX_NAMESPACE/w/pagelayoutmanager.H>

factory->create_container([]
                          (const auto &new_container)
                          {
                              INSERT_LIBX_NAMESPACE::w::pagelayoutmanager layout_manager=new_container->get_layoutmanager();

                              // ...
                          },
                          INSERT_LIBX_NAMESPACE::w::new_pagelayoutmanager{});

\endcode

This layout manager treats each element as a virtual page, with only one
page being visible at any given time. Making another page, another element,
another page visible hides the currently visible page, which gets replaced
by the new page.

\code
#include <INSERT_LIBX_NAMESPACE/w/pagefactory.H>

INSERT_LIBX_NAMESPACE::w::pagefactory factory=layout_manager->append();

auto label=factory->create_label("Hello world")->show();

INSERT_LIBX_NAMESPACE::w::pagefactory factory=layout_manager->insert(0);

layout_manager->remove(1);

\endcode

The pages get numbered by the page layout manager, starting with page #0.
append() returns a \ref pagefactory "factory" that adds new pages, new
widgets, to the end of the existing list of pages; insert() returns
 a factory that inserts new pages between an existing page, identified
by its number, and automatically renumbers the remaining pages.
remove() removes a page, and automatically renumbers the remaining pages.

\note
Each page, each widget in the page container must still be show()n
in order to be visible when its turn comes to shine, even though
the page layout manager only makes one page visible.

lookup() returns the current page number of the given element.

\code
std::optional<size_t> layout_manager->lookup(label);

layout_manager->open(layout_manager->size()-1);

layout_manager->close();

std::optional<size_t> n=layout_manager->opened();

INSERT_LIBX_NAMESPACE::w::element e=layout_manager->get(0);

\endcode

size() returns the number of pages in the container container.
lookup() returns the index of the given element. lookup() returns nothing
if the given element is not in the container: you looked up someone else's
element.

open() makes the given page number (the element-pages are numbered starting
with 0) visible, and makes the currently-visible page, if there is one,
disappear.

close() makes the currently visible page disappear, without making another
page visible. opened() indicates which page is currently visible, if there
is one.

get() returns element \#n.

\code
INSERT_LIBX_NAMESPACE::w::page_lock lock{layout_manager};
\endcode

Constructing the page lock object blocks all other execution threads
from accessing the page layout manager. The number of elements in the
container, and other aspects of the container, may be examined using
the page layout manager as a stable, consistent snapshot, blocking
other execution threads from modifying the paged container.

\note
This also includes the library's internal execution thread. An existing
page lock can block the internal execution thread from updating the
display.

*/

typedef ref<pagelayoutmanagerObj> pagelayoutmanager;

//! A constant \ref pagelayoutmanager "page layout manager".

//! \see pagelayoutmanager

typedef const_ref<pagelayoutmanagerObj> const_pagelayoutmanager;

//! A nullable pointer reference to a \ref pagelayoutmanager "page layout manager".

//! \see pagelayoutmanager

typedef ptr<pagelayoutmanagerObj> pagelayoutmanagerptr;

//! A nullable pointer reference to a const \ref pagelayoutmanager "page layout manager".

//! \see pagelayoutmanager

typedef const_ptr<pagelayoutmanagerObj> const_pagelayoutmanagerptr;

struct LIBCXX_HIDDEN page_layout_info_s;

//! Internal page layout manager mutex-protected info.

//! \internal
typedef mpobj<page_layout_info_s, std::recursive_mutex> page_layout_info_t;


LIBCXXW_NAMESPACE_END

#endif
