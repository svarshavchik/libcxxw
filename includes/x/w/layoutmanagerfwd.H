/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_layoutmanagerfwd_h
#define x_w_layoutmanagerfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class layoutmanagerObj;

//! A layout manager

//! Every container has a layout manager, that manages the elements inside
//! the container element.

typedef ref<layoutmanagerObj> layoutmanager;

//! A constant generic layout manager.

//! \see layoutmanager

typedef const_ref<layoutmanagerObj> const_layoutmanager;

LIBCXXW_NAMESPACE_END

#endif
