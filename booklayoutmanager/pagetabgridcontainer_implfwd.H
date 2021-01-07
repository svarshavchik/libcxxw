/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_pagetabgridcontainer_implfwd_h
#define x_w_pagetabgridcontainer_implfwd_h

#include "x/w/impl/always_visiblefwd.H"
#include "x/w/impl/container_elementfwd.H"
#include "x/w/impl/child_element.H"
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

//! The container for the book's page tabs.

typedef always_visibleObj<
	container_elementObj<child_elementObj>
	> pagetabgridcontainer_implObj;

/*! Container for \ref booklayoutmanager "booklayoutmanager"'s tab strip.

This container uses the \ref pagetabgridlayoutmanagerObj "pagetabgridlayoutmanagerObj".

*/

typedef ref<pagetabgridcontainer_implObj> pagetabgridcontainer_impl;

//! A constant \ref pagetabgridcontainer_impl "booklayoutmanager page strip container".

//! \see pagetabgridcontainer_impl

typedef const_ref<pagetabgridcontainer_implObj> const_pagetabgridcontainer_impl;

//! A nullable pointer reference to a \ref pagetabgridcontainer_impl "booklayoutmanager page strip container".

//! \see pagetabgridcontainer_impl

typedef ptr<pagetabgridcontainer_implObj> pagetabgridcontainer_implptr;

//! A nullable pointer reference to a const \ref pagetabgridcontainer_impl "booklayoutmanager page strip container".

//! \see pagetabgridcontainer_impl

typedef const_ptr<pagetabgridcontainer_implObj> const_pagetabgridcontainer_implptr;

LIBCXXW_NAMESPACE_END

#endif
