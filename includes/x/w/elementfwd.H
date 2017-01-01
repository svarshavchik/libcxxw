/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_elementfwd_h
#define x_w_elementfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class elementObj;

//! Methods common to all display elements.

typedef ref<elementObj> element;

//! A constant generic window handler

//! \see element

typedef const_ref<elementObj> const_element;

LIBCXXW_NAMESPACE_END

#endif
