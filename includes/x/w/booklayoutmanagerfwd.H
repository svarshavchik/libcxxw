/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_booklayoutmanagerfwd_h
#define x_w_booklayoutmanagerfwd_h

#include <x/w/namespace.H>
#include <x/w/busyfwd.H>
#include <x/mpobj.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN new_booklayoutmanager;
class LIBCXX_PUBLIC booklayoutmanagerObj;
struct LIBCXX_PUBLIC book_status_info_t;
class LIBCXX_PUBLIC book_lock;

/*! The book layout manager.

\code

auto container=factory->create_focusable_container
    ([]
     (const INSERT_LIBX_NAMESPACE::w::container &c)
     {
          INSERT_LIBX_NAMESPACE::w::booklayoutmanager book=c->get_layoutmanager();

          // ...
     },
     INSERT_LIBX_NAMESPACE::w::new_booklayoutmanager
\endcode

\ref new_booklayoutmanager "INSERT_LIBX_NAMESPACE::w::new_booklayoutmanager"
creates a container with the book layoutmanager, which is an enhanced version
of the \ref pagelayoutmanager "page layout manager". Like
the \c pagelayoutmanager, the book layout manager arranges its elements
in individual pages, with one page visible at a time. On top of that,
the book layout manager adds a row of tabs above the currently shown page,
one tab for each page in the book; and clicking on a page's tab opens that
page.

Also, horizontal scrolling arrows appear on the left and the right side of the
tab row. They, respectively, open the previous and the next page.

\code
INSERT_LIBX_NAMESPACE::w::book_lock lock{book};

INSERT_LIBX_NAMESPACE::w::bookpagefactory factory=book->append();

INSERT_LIBX_NAMESPACE::w::bookpagefactory factory=book->insert(0);

INSERT_LIBX_NAMESPACE::w::element e=book->get_page(0);

book->remove(0);

size_t n=book->pages();

book->open(1);

book->close();

std::optional<size_t> n=book->opened();

\endcode

A \ref book_lock "INSERT_LIBX_NAMESPACE::w::book_lock" objects blocks all
other execution threads from accessing the book layout manager's container,
while the lock exists.

append() returns a \ref bookpagefactory "factory" that adds new pages to
the book after all existing pages.
insert() returns a \ref bookpagefactory "factory" that adds new pages to
the book before an existing page, and remove() removes a page from the book.
Like the page layout manager, all pages in the book get automatically
numbered, starting with 0 for the first page; and the pages get automatically
renumbered when pages get added or removed from the book.

pages() returns the number of pages in the book. open() makes another page
visible instead of the current one. close() closes the currently visible
page without showing any other page from the book. opened() indicates which
page is currently open.

\code
book->on_opened([]
                (ONLY IN_THREAD,
                 const INSERT_LIBX_NAMESPACE::w::book_status_info_t &info)
                {
                     size_t n=info.opened;
                });
\endcode

on_opened() installs a callback that gets executed whenever the book
layout manager opens a new page, either as a result of selecting one of
the tabs, or by a call to open().

*/

typedef ref<booklayoutmanagerObj> booklayoutmanager;

//! A constant \ref booklayoutmanager "book layout manager".

//! \see booklayoutmanager

typedef const_ref<booklayoutmanagerObj> const_booklayoutmanager;

//! A nullable pointer reference to a \ref booklayoutmanager "book layout manager".

//! \see booklayoutmanager

typedef ptr<booklayoutmanagerObj> booklayoutmanagerptr;

//! A nullable pointer reference to a const \ref booklayoutmanager "book layout manager".

//! \see booklayoutmanager

typedef const_ptr<booklayoutmanagerObj> const_booklayoutmanagerptr;

LIBCXXW_NAMESPACE_END

#endif
