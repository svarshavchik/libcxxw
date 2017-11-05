/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_menubarfactoryfwd_h
#define x_w_menubarfactoryfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class menubarfactoryObj;

/*! Menu bar factory.

A factory for adding menus to a \ref menubarlayoutmanager "menu bar".

\code
factory->add([]
             (const INSERT_LIBX_NAMESPACE::w::factory &f)
             {
             },
             []
             (const INSERT_LIBX_NAMESPACE::w::listlayoutmanager &m)
             {
             });

factory->add_text("File",
                  []
                  (const INSERT_LIBX_NAMESPACE::w::listlayoutmanager &m)
                  {
                  });
\endcode

\c INSERT_LIBX_NAMESPACE::w::menubarfactory is a meta-factory that adds
new menus to the application's menu bar. The title of the new menu does
not necessarily have to be a text string. Any non-focusable display
element can serve as a menu title. add() creates a new menu. The first
parameter is a callback that receives an \c INSERT_LIBX_NAMESPACE::w::factory
for a parameter, and should use it to construct a display element that
becomes the new menu's title. add_text()'s first parameter is a
\ref text_param "INSERT_LIBX_NAMESPACE::w::text_param". add_text() is
equivalent to using add() with create_label(). Only one display element
may be created using the factory, and it gets automatically show()n.

The second parameter is the creator callback for the menu's contents.
The creator callback receives an
\ref listlayoutmanager "INSERT_LIBX_NAMESPACE::w::listlayoutmanager"
parameter, which it should use to populate the new menu.

*/

typedef ref<menubarfactoryObj> menubarfactory;

//! A constant \ref menubarfactory "menu bar factory".

//! \see menubarfactory

typedef const_ref<menubarfactoryObj> const_menubarfactory;

//! A nullable pointer reference to a \ref menubarfactory "menu bar factory".

//! \see menubarfactory

typedef ptr<menubarfactoryObj> menubarfactoryptr;

//! A nullable pointer reference to a const \ref menubarfactory "menu bar factory".

//! \see menubarfactory

typedef const_ptr<menubarfactoryObj> const_menubarfactoryptr;

LIBCXXW_NAMESPACE_END

#endif
