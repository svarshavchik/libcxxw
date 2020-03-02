/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_bookpagefactoryfwd_h
#define x_w_bookpagefactoryfwd_h

#include <x/w/labelfwd.H>
#include <x/w/shortcutfwd.H>

#include <x/ptrfwd.H>
#include <x/optional_argsfwd.H>

LIBCXXW_NAMESPACE_START

//! The options for a \ref bookpagefactory "book page factory"'s add().

typedef optional_argconstrefs<label_config, shortcut> create_bookpage_with_label_args_t;

//! The options for a \ref bookpagefactory "book page factory"'s add().

typedef optional_argconstrefs<shortcut> create_bookpage_args_t;

class LIBCXX_PUBLIC bookpagefactoryObj;

/*! A factory that adds another page to a container with a \ref booklayoutmanager "book layout manager".

\code

factory->halign(LIBCXX_NAMESPACE:w::halign::left)
        .valign(LIBCXX_NAMESPACE:w::halign::top)
        .add("Page 1",
             []
             (const LIBCXX_NAMESPACE::w::factory &page_factory)
            {
               page_factory->create_container([]
                                              (const auto &c)
                                              {
                                              },
                                              INSERT_LIBX_NAMESPACE::w::new_gridlayoutmanager{})->show();
               // ...
            });

factory->appearance=theme;
factory->set_appearance(theme)

factory->add([]
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

Each page in a book consists of a tab, shown above the page container,
and the page itself, a widget.

When a book's pages have different sizes, the smaller pages get centered
in the larger book by default. add() can be called more than once, adding
multiple pages to the book at a time. halign() and valign() overrides the
alignment only for the next add()ed page.

The factory's \c appearance class member specifies the visual appearance
of the new pages' tabs.

set_appearance() installs a new \c appearance member, and is equivalent to
setting the \c appearance class member directly.

add() is overloaded. The first version of add takes the following parameters:

- \ref text_param "INSERT_LIBX_NAMESPACE::w::text_param" sets the page's label
shown in the page's tab, as a text string.

- A closure or a callable object. The callable object receives one parameter,
a \ref factory "factory" that the callable object must use to create one widget that represents the page in the book; typically a
\ref container "container". The callable object is responsible for
show()ing the new element.

- An optional \ref shortcut "keyboard shortcut" for opening the new page.

- An optional \ref halign "alignment" for the text tab, if it contains
multiple lines of text. Even though the book layout manager
controls which page is currently visible, like all other widgets,
they have to be shown, in order to actually become visible when the page
gets shown.

The page's tab is nearly always a short text label but any non-focusable
widget can serve as a tab. This is done by using the second version
of the overloaded add() without the label's parameter; and the callable
object receives two factory objects as its parameters instead of one.
The first factory is for creating the widget that represents
the new page's; which may even a container with multiple
elements (although big tabs won't look very pretty). The second factory
is for creating the new page's element.

The closure must invoke both of the factories exactly once, using each
factory to create one widget. The callable object is responsible
for show()ing both new elements.

\code
factory->noncurrent_color=INSERT_LIBX_NAMESPACE::w::silver;

factory->current_color=INSERT_LIBX_NAMESPACE::w::white;

factory->warm_color=INSERT_LIBX_NAMESPACE::w::yellow;

factory->active_color=INSERT_LIBX_NAMESPACE::w::aqua;

factory->label_font=INSERT_LIBX_NAMESPACE::w::font{"liberation mono"};

factory->active_color=INSERT_LIBX_NAMESPACE::w::black;

factory->horiz_padding=2.0;
factory->vert_padding=2.0;
\endcode

The factory initializes several (non thread-safe) members to the default
appearance of each new tab, from the current display theme; which may be
updated if needed.

Since the default appearance is theme controlled, the best approach involves
specifying all these theme-specified values (together with a few additional
settings in the
\ref new_booklayoutmanager "INSERT_LIBX_NAMESPACE::w::new_booklayoutmanager").

\see booklayoutmanager

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
