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
//! another display element, using focusframelayoutimplObj.
//!
//! This is mostly an internal object. The focus frame container is derived
//! from a generic container object, but its layout manager should not be
//! accessed directly. The focus frame container takes care of the layout
//! manager. get_focusable() returns the display element in the container.
//!
//! set_focusable() returns a factory object which will install a
//! new display element as the underlying new element.
//!
//! The focusframe "public" object does not really have to be created for
//! the implementation object. There are two general possibilities:
//!
//! 1) The focusframecontainer_element mixin is combined with a container
//! mixin that itself is combined with a focusable element, such as a hotspot.
//! In this case the focusframecontainer "public" object gets used, and it
//! is the focusable owner of the implementation focusframecontainer object.
//! The contents of the container are user-replaceable, and cannot be
//! focusable objects.
//!
//! 2) The focusframecontainer_element mix is combined with a container with a
//! focusable element, and some other public object that's set as the
//! focusable owner. The focusframelayoutimplObj layout manager must be used
//! as the focusframecontainerObj::implObj's container.

typedef ref<focusframecontainerObj> focusframecontainer;

//! A constant generic window handler

//! \see focusframecontainer

typedef const_ref<focusframecontainerObj> const_focusframecontainer;

LIBCXXW_NAMESPACE_END

#endif
