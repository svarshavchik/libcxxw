/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_switchfactoryfwd_h
#define x_w_switchfactoryfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class switchfactoryObj;

/*! Factory for containers managed by the \ref switchlayoutmanager "switch layout manager".

This factory is returned by \ref switchlayoutmanager "INSERT_LIBX_NAMESPACE::w::switchlayoutmanager"'s
append() and insert() methods. It inherits from the
\ref factory "standard factory object", and provides the additional
halign() and valign() methods.

The switch layout manager automatically sizes the container to accomodate
the largest element in the container, even when it is visible; so switching
to a different element does not change the layout of the display elements
in the window.

Other, smaller elements in the switched container appear in the center of
the container by default. When creating new display element, invoking
halign() and/or valign() specifies that the next display element created
by the factory gets positioned in the switched container, when the element
is visible, according to the
specified alignment if it's not the largest element in the switch.

*/

typedef ref<switchfactoryObj> switchfactory;

//! A constant \ref switchfactory "element factory for the switch layout manager".

//! \see switchfactory

typedef const_ref<switchfactoryObj> const_switchfactory;

//! A nullable pointer reference to a \ref switchfactory "element factory for the switch layout manager".

//! \see switchfactory

typedef ptr<switchfactoryObj> switchfactoryptr;

//! A nullable pointer reference to a const \ref switchfactory "element factory for the switch layout manager".

//! \see switchfactory

typedef const_ptr<switchfactoryObj> const_switchfactoryptr;

LIBCXXW_NAMESPACE_END

#endif
