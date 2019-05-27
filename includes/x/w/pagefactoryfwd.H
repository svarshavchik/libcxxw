/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_pagefactoryfwd_h
#define x_w_pagefactoryfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC pagefactoryObj;

/*! Factory for containers managed by the \ref pagelayoutmanager "page layout manager".

This factory is returned by \ref pagelayoutmanager "INSERT_LIBX_NAMESPACE::w::pagelayoutmanager"'s
append() and insert() methods. It inherits from the
\ref factory "standard factory object", and provides the additional
halign() and valign() methods.

The page layout manager automatically sizes the container to accomodate
the largest element in the container, even when it is visible; so switching
to a different element does not change the layout of the widgets
in the window.

Other, smaller elements in the switched container appear in the center of
the container by default. When creating new widget, invoking
halign() and/or valign() specifies that the next widget created
by the factory gets positioned in the switched container, when the element
is visible, according to the
specified alignment if it's not the largest element in the switch.

*/

typedef ref<pagefactoryObj> pagefactory;

//! A constant \ref pagefactory "element factory for the page layout manager".

//! \see pagefactory

typedef const_ref<pagefactoryObj> const_pagefactory;

//! A nullable pointer reference to a \ref pagefactory "element factory for the page layout manager".

//! \see pagefactory

typedef ptr<pagefactoryObj> pagefactoryptr;

//! A nullable pointer reference to a const \ref pagefactory "element factory for the page layout manager".

//! \see pagefactory

typedef const_ptr<pagefactoryObj> const_pagefactoryptr;

LIBCXXW_NAMESPACE_END

#endif
