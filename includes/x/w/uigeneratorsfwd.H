/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_uigeneratorsfwd_h
#define x_w_uigeneratorsfwd_h

#include <x/w/namespace.H>
#include <x/w/gridlayoutmanagerfwd.H>
#include <x/w/booklayoutmanagerfwd.H>
#include <x/w/gridfactoryfwd.H>
#include <x/w/bookpagefactoryfwd.H>
#include <x/w/uielementsfwd.H>
#include <x/w/rgbfwd.H>
#include <x/functionalrefptrfwd.H>

LIBCXXW_NAMESPACE_START

//! A theme color is either an rgb solid color, or a gradient color.

typedef std::variant<rgb, linear_gradient, radial_gradient> theme_color_t;

//! Generate something in the grid layout manager

//! \see uigenerators

typedef functionref<void (const gridlayoutmanager &, uielements &)
		    > gridlayoutmanager_generator;

//! Generate something using a grid factory.

//! \see uigenerators
typedef functionref<void (const gridfactory &, uielements &)
		    > gridfactory_generator;

//! Generate something with the book layout manager

//! \see uigenerators
typedef functionref<void (const booklayoutmanager &, uielements &)
		    > booklayoutmanager_generator;

//! Generate something using a book page factory.

//! \see uigenerators
typedef functionref<void (const bookpagefactory &, uielements &)
		    > bookpagefactory_generator;

class LIBCXX_PUBLIC uigeneratorsObj;
class LIBCXX_PUBLIC uigeneratorsBase;

/*! Loaded and parsed theme files

XML-based specification that's compiled into a form that gets used to
generate() the contents of supported layout manager (and factories).

\code
INSERT_LIBX_NAMESPACE::w::const_uigenerators generators=INSERT_LIBX_NAMESPACE::w::uigenerators::create("dialogs.xml");
\endcode

create() always returns a \c const_uigenerators, the created object is
always constant, and represents the parsed XML layout that gets loaded
from an XML file.

\code
INSERT_LIBX_NAMESPACE::w::uielements factories{
    {
          {"label",
           [&]
           (const INSERT_LIBX_NAMESPACE::w::factory &f)
           {
                  f->create_label("Lorem ipsum");
           }
          },
          {"button",
           [&]
           (const INSERT_LIBX_NAMESPACE::w::factory &f)
           {
                  f->create_button("Lorem ipsum");
           }
          },
    }
};

INSERT_LIBX_NAMESPACE::w::gridlayoutmanager gml=container->get_layoutmanager();

gml->generate("main", generators, elements);
\endcode

A \ref gridlayoutmanager "grid layout manager"'s generate() creates the
contents of its container from the loaded theme file and the
\ref uielements "widget factories".

See the tutorial for more information on XML-specified generators.

*/

typedef ref<uigeneratorsObj, uigeneratorsBase> uigenerators;

//! Constant \ref uigenerators "compiled widget generators".

//! \see uigenerators

typedef const_ref<uigeneratorsObj, uigeneratorsBase> const_uigenerators;

//! A nullable pointer reference to \ref uigenerators "compiled widget generators".

//! \see uigenerators

typedef ptr<uigeneratorsObj, uigeneratorsBase> uigeneratorsptr;

//! A nullable pointer reference to const \ref uigenerators "compiled widget generators".

//! \see uigenerators

typedef const_ptr<uigeneratorsObj, uigeneratorsBase> const_uigeneratorsptr;

LIBCXXW_NAMESPACE_END

#endif
