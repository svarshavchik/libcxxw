/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef listcontainerfwd_h
#define listcontainerfwd_h

#include "x/w/namespace.H"
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN listcontainerObj;

//! A container with a listlayoutmanager

//! An internal, focusable container. Handles keyboard and pointer events.
//! using the list layout manager.
//!
//! Not a focus owner. This is placed inside a peepholed focusframe, which
//! owns this focusable.
//!
//! \see listlayoutmanager

typedef ref<listcontainerObj> listcontainer;

//! A nullable pointer reference to a container with a listlayoutmanager

//! \see listcontainer

typedef ptr<listcontainerObj> listcontainerptr;

LIBCXXW_NAMESPACE_END

#endif
