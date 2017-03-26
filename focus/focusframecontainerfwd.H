/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef focusframecontainerfwd_h
#define focusframecontainerfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class focusframecontainerObj;

//! A focus frame container.

//! A container that draws a focus frame around
//! another display element.
//!
//! This is mostly an internal object. The focus frame container is derived
//! from a generic container object, but its layout manager should not be
//! accessed directly. The focus frame container takes care of the layout
//! manager. get_focusable() returns the display element in the container.
//! replace_focusable() installs another one.
//!
//! replace_focusable() returns a factory object which will install a
//! new display element as the underlying new element.

typedef ref<focusframecontainerObj> focusframecontainer;

//! A constant generic window handler

//! \see focusframecontainer

typedef const_ref<focusframecontainerObj> const_focusframecontainer;

LIBCXXW_NAMESPACE_END

#endif
