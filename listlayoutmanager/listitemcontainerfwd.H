/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef listitemcontainerfwd_h
#define listitemcontainerfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class listitemcontainerObj;

struct listitem_sharedstateObj;

//! A container that uses the listitemlayoutmanager.

//! The actual element that gets added to a grid container that pinch-hits
//! as a list container.
//!
//! This container always has one element, and uses the listitemlayoutmanager
//! to manages it.
//!
//! The list layout manager adds this container to its underlying grid,
//! instead of the list display element, and the list display element is
//! the only element in this container.
//!
//! The list item layout manager takes care of positioning the display element.
//!
//! \see listlayoutmanager

typedef ref<listitemcontainerObj> listitemcontainer;

//! A nullable pointer reference to a container with a listitemlayoutmanager.

//! \see listitemcontainer

typedef ptr<listitemcontainerObj> listitemcontainerptr;

LIBCXXW_NAMESPACE_END

#endif
