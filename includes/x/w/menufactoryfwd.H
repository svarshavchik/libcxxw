/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_menufactoryfwd_h
#define x_w_menufactoryfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class menufactoryObj;

/*! A factory for adding new items to a \ref menulayoutmanager "menu".

\code
INSERT_LIBX_NAMESPACE::w::menuitem_plain menuitem_type;

f->set_type(menuitem_type);
f->create_label("About");
\endcode

A menu item element is typically a text \c INSERT_LIBX_NAMESPACE::w::label
but it can be any non-focusable display element. set_type() gets called
before creating the new display element, and sets the type
for the new menu item.

set_type() specifies the type of the next menu item only, as a
\ref menuitem_type_t "INSERT_LIBX_NAMESPACE::w::menuitem_type_t".
A single factory can create more than one menu item, but set_type() specifies
only the next item's type, and needs to get called again before creating
the next menu item. Without a preceding set_type(), the new menu item
gets created as a \ref menuitem_plain "INSERT_LIBX_NAMESPACE::w::menuitem_plain"
with all default options.

*/

typedef ref<menufactoryObj> menufactory;

//! A constant \ref menufactory "menu factory".

//! \see menufactory

typedef const_ref<menufactoryObj> const_menufactory;

//! A nullable pointer reference to a \ref menufactory "menu factory".

//! \see menufactory

typedef ptr<menufactoryObj> menufactoryptr;

//! A nullable pointer reference to a const \ref menufactory "menu factory".

//! \see menufactory

typedef const_ptr<menufactoryObj> const_menufactoryptr;

LIBCXXW_NAMESPACE_END

#endif
