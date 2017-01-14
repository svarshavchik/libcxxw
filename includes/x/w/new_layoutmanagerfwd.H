/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_new_layoutmanagerfwd_h
#define x_w_new_layoutmanagerfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class new_layoutmanagerObj;
class new_layoutmanagerBase;

//! A new layout manager

//! A new container's layout manager gets created before the
//! \ref container "container" gets created.
//!
//! A layout manager factory returns this new_layoutmanager object, which
//! gets used to construct the new container.
//!
//! After the container gets created, this object must be destroyed.

typedef ref<new_layoutmanagerObj, new_layoutmanagerBase> new_layoutmanager;

//! A constant generic window handler

//! \see new_layoutmanager

typedef const_ref<new_layoutmanagerObj,
		  new_layoutmanagerBase> const_new_layoutmanager;

LIBCXXW_NAMESPACE_END

#endif
