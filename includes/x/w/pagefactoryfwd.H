/*
** Copyright 2017-2020 Double Precision, Inc.
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
\ref factory "standard factory object", and implements additional
halign() and valign() methods.

The page layout manager automatically sizes the container to accomodate
the largest widget in the container, even when it's not visible; so switching
to a different widget does not change the layout of other widgets
in the window.

Calling halign() and valign() before creating new widgets specifies how
narrow or shorter widges get accomodated in the page that's sized for
the largest widget. By default all widgets get centered on their respective
pages.

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
