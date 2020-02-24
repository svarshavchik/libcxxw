/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_uigeneratorsfwd_h
#define x_w_uigeneratorsfwd_h

#include <x/w/namespace.H>
#include <x/w/factoryfwd.H>
#include <x/w/gridlayoutmanagerfwd.H>
#include <x/w/booklayoutmanagerfwd.H>
#include <x/w/listlayoutmanagerfwd.H>
#include <x/w/menubarlayoutmanagerfwd.H>
#include <x/w/custom_comboboxlayoutmanagerfwd.H>
#include <x/w/standard_comboboxlayoutmanagerfwd.H>
#include <x/w/editable_comboboxlayoutmanagerfwd.H>
#include <x/w/tablelayoutmanagerfwd.H>
#include <x/w/panelayoutmanagerfwd.H>
#include <x/w/panefactoryfwd.H>
#include <x/w/menubarfactoryfwd.H>
#include <x/w/gridfactoryfwd.H>
#include <x/w/bookpagefactoryfwd.H>
#include <x/w/pagefactoryfwd.H>
#include <x/w/itemlayoutmanagerfwd.H>
#include <x/w/pagelayoutmanagerfwd.H>
#include <x/w/toolboxlayoutmanagerfwd.H>
#include <x/w/toolboxfactoryfwd.H>
#include <x/w/borderlayoutmanagerfwd.H>
#include <x/w/uielementsfwd.H>
#include <x/w/rgbfwd.H>
#include <x/functionalrefptrfwd.H>

LIBCXXW_NAMESPACE_START

//! A theme color is either an rgb solid color, or a gradient color.

typedef std::variant<rgb, linear_gradient, radial_gradient> theme_color_t;

//! HTML 3.2 rgb color names

extern const char * const rgb_color_names[];

//! HTML 3.2 rgb colors
extern const rgb rgb_colors[];

//! How many rgb_colors there are.
extern const size_t n_rgb_colors;

//! Generate something using a base factory.

//! \see uigenerators
typedef functionref<void (const factory &, uielements &)
		    > factory_generator;

//! Generate something in the grid layout manager

//! \see uigenerators

typedef functionref<void (const gridlayoutmanager &, uielements &)
		    > gridlayoutmanager_generator;

//! Generate something in the list layout manager

//! \see uigenerators

typedef functionref<void (const listlayoutmanager &, uielements &)
		    > listlayoutmanager_generator;

//! Generate something in the table layout manager

//! \see uigenerators

typedef functionref<void (const tablelayoutmanager &, uielements &)
		    > tablelayoutmanager_generator;

//! Generate something in the pane layout manager

//! \see uigenerators

typedef functionref<void (const panelayoutmanager &, uielements &)
		    > panelayoutmanager_generator;

//! Generate something in the item layout manager

//! \see uigenerators

typedef functionref<void (const itemlayoutmanager &, uielements &)
		    > itemlayoutmanager_generator;

//! Generate something in the page layout manager

//! \see uigenerators

typedef functionref<void (const pagelayoutmanager &, uielements &)
		    > pagelayoutmanager_generator;

//! Generate something in the toolbox layout manager

//! \see uigenerators

typedef functionref<void (const toolboxlayoutmanager &, uielements &)
		    > toolboxlayoutmanager_generator;

//! Generate something in the standard combobox layout manager

//! \see uigenerators

typedef functionref<void (const standard_comboboxlayoutmanager &, uielements &)
		    > standard_comboboxlayoutmanager_generator;

//! Generate something in the editable combobox layout manager

//! \see uigenerators

typedef functionref<void (const editable_comboboxlayoutmanager &, uielements &)
		    > editable_comboboxlayoutmanager_generator;

//! Generate something using a grid factory.

//! \see uigenerators
typedef functionref<void (const gridfactory &, uielements &)
		    > gridfactory_generator;

//! Generate something using a menu bar layout manager.

//! \see uigenerators
typedef functionref<void (const menubarlayoutmanager &, uielements &)
		    > menubarlayoutmanager_generator;

//! Generate something using a menu bar factory.

//! \see uigenerators
typedef functionref<void (const menubarfactory &, uielements &)
		    > menubarfactory_generator;

//! Generate something with the book layout manager

//! \see uigenerators
typedef functionref<void (const booklayoutmanager &, uielements &)
		    > booklayoutmanager_generator;

//! Generate something using a book page factory.

//! \see uigenerators
typedef functionref<void (const bookpagefactory &, uielements &)
		    > bookpagefactory_generator;

//! Generate something using a book factory.

//! \see uigenerators
typedef functionref<void (const pagefactory &, uielements &)
		    > pagefactory_generator;

//! Generate something using a pane factory.

//! \see uigenerators
typedef functionref<void (const panefactory &, uielements &)
		    > panefactory_generator;

//! Generate something using a toolbox factory.

//! \see uigenerators
typedef functionref<void (const toolboxfactory &, uielements &)
		    > toolboxfactory_generator;

//! Generate something in the border layout manager

//! \see uigenerators

typedef functionref<void (const borderlayoutmanager &, uielements &)
		    > borderlayoutmanager_generator;

//! Pseudo-ref used to initialize a new_booklayoutmanager parameter

typedef new_booklayoutmanager *new_booklayoutmanager_plainptr;

//! Generate something to initialize a new_booklayoutmanager

//! \see uigenerators
typedef functionref<void (const new_booklayoutmanager_plainptr &layout,
			  uielements &elements)
		    > new_booklayoutmanager_generator;

//! Pseudo-ref used to initialize a new_listlayoutmanager parameter

typedef new_listlayoutmanager *new_listlayoutmanager_plainptr;

//! Generate something to initialize a new_listlayoutmanager

//! \see uigenerators
typedef functionref<void (const new_listlayoutmanager_plainptr &layout,
			  uielements &elements)
		    > new_listlayoutmanager_generator;

//! Pseudo-ref used to initialize a new_custom_comboboxlayoutmanager parameter

typedef new_custom_comboboxlayoutmanager
*new_custom_comboboxlayoutmanager_plainptr;

//! Generate something to initialize a new_custom_comboboxlayoutmanager

//! \see uigenerators
typedef functionref<void (const new_custom_comboboxlayoutmanager_plainptr &,
			  uielements &elements)
		    > new_custom_comboboxlayoutmanager_generator;

//! Pseudo-ref used to initialize a new_standard_comboboxlayoutmanager parameter

typedef new_standard_comboboxlayoutmanager
*new_standard_comboboxlayoutmanager_plainptr;

//! Generate something to initialize a new_standard_comboboxlayoutmanager

//! \see uigenerators
typedef functionref<void (const new_standard_comboboxlayoutmanager_plainptr &,
			  uielements &elements)
		    > new_standard_comboboxlayoutmanager_generator;

//! Pseudo-ref used to initialize a new_editable_comboboxlayoutmanager parameter

typedef new_editable_comboboxlayoutmanager
*new_editable_comboboxlayoutmanager_plainptr;

//! Generate something to initialize a new_editable_comboboxlayoutmanager

//! \see uigenerators
typedef functionref<void (const new_editable_comboboxlayoutmanager_plainptr &,
			  uielements &elements)
		    > new_editable_comboboxlayoutmanager_generator;

//! Pseudo-ref used to initialize a new_tablelayoutmanager parameter

typedef new_tablelayoutmanager *new_tablelayoutmanager_plainptr;

//! Generate something to initialize a new_tablelayoutmanager

//! \see uigenerators
typedef functionref<void (const new_tablelayoutmanager_plainptr &layout,
			  uielements &elements)
		    > new_tablelayoutmanager_generator;

//! Pseudo-ref used to initialize a new_panelayoutmanager parameter

typedef new_panelayoutmanager *new_panelayoutmanager_plainptr;

//! Generate something to initialize a new_panelayoutmanager

//! \see uigenerators
typedef functionref<void (const new_panelayoutmanager_plainptr &layout,
			  uielements &elements)
		    > new_panelayoutmanager_generator;

//! Pseudo-ref used to initialize a new_itemlayoutmanager parameter

typedef new_itemlayoutmanager *new_itemlayoutmanager_plainptr;

//! Generate something to initialize a new_itemlayoutmanager

//! \see uigenerators
typedef functionref<void (const new_itemlayoutmanager_plainptr &layout,
			  uielements &elements)
		    > new_itemlayoutmanager_generator;

//! Pseudo-ref used to initialize a new_pagelayoutmanager parameter

typedef new_pagelayoutmanager *new_pagelayoutmanager_plainptr;

//! Generate something to initialize a new_pagelayoutmanager

//! \see uigenerators
typedef functionref<void (const new_pagelayoutmanager_plainptr &layout,
			  uielements &elements)
		    > new_pagelayoutmanager_generator;

//! Pseudo-ref used to initialize a new_toolboxlayoutmanager parameter

typedef new_toolboxlayoutmanager *new_toolboxlayoutmanager_plainptr;

//! Generate something to initialize a new_toolboxlayoutmanager

//! \see uigenerators
typedef functionref<void (const new_toolboxlayoutmanager_plainptr &layout,
			  uielements &elements)
		    > new_toolboxlayoutmanager_generator;

//! Pseudo-ref used to initialize a new_borderlayoutmanager parameter

typedef new_borderlayoutmanager *new_borderlayoutmanager_plainptr;

//! Generate something to initialize a new_borderlayoutmanager

//! \see uigenerators
typedef functionref<void (const new_borderlayoutmanager_plainptr &layout,
			  uielements &elements)
		    > new_borderlayoutmanager_generator;

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

A layout manager's generate() creates the
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
