/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_bookpagefactoryfwd_h
#define x_w_bookpagefactoryfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class bookpagefactoryObj;

/*! A factory that adds another page to a container that uses the \ref booklayoutmanager "book layout manager".

\code

factory->halign(LIBCXX_NAMESPACE:w::halign::left)
        .valign(LIBCXX_NAMESPACE:w::halign::top)
        .add([]
             (const LIBCXX_NAMESPACE::w::factory &tab_factory,
              const LIBCXX_NAMESPACE::w::factory &page_factory)
            {
               tab_factory->create_label("Page 1")->show();
               page_factory->create_container([]
                                              (const auto &c)
                                              {
                                              },
                                              INSERT_LIBX_NAMESPACE::w::new_gridlayoutmanager{})->show();
               // ...
            });
\endcode

When a book's pages have different sizes, the smaller pages get cenetered
in the larger book by default. add() can be called more than once, adding
multiple pages to the book at a time. halign() and valign() overrides the
alignment only for the next add()ed page.

add() receives a closure or a callable object as a parameter. The callable
object receives two parameters, two factories. The first factory is for
creating the page's tab, which is typically a \ref label "label", but can
be any non-focusable display element; even a container with multiple
elements (although big tabs won't look very pretty). The second factory
creates the contents of the new page.

The closure must invoke both of the factories exactly once, using each
factory to create one display element. Like all other display elements,
both new elements must be show()n. Even though the book layout manager
controls which page is currently visible, like all other display elements,
they have to be shown in order to actually become visible when the page
gets shown.

\see switchlayoutmanager

*/

typedef ref<bookpagefactoryObj> bookpagefactory;

//! A constant \ref bookpagefactory "book page factory".

//! \see bookpagefactory

typedef const_ref<bookpagefactoryObj> const_bookpagefactory;

//! A nullable pointer reference to a \ref bookpagefactory "book page factory".

//! \see bookpagefactory

typedef ptr<bookpagefactoryObj> bookpagefactoryptr;

//! A nullable pointer reference to a const \ref bookpagefactory "book page factory".

//! \see bookpagefactory

typedef const_ptr<bookpagefactoryObj> const_bookpagefactoryptr;

LIBCXXW_NAMESPACE_END

#endif
