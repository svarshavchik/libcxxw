/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef straight_borderfwd_h
#define straight_borderfwd_h

#include <x/ptrfwd.H>
#include <x/w/namespace.H>

LIBCXXW_NAMESPACE_START

class straight_borderObj;
class straight_borderBase;

//! A straight border element in a grid

//! This is an internal element object that's automatically created by
//! \ref gridlayoutmanager "grid layout manager" for drawing horizontal
//! and vertical grid borders.

typedef ref<straight_borderObj, straight_borderBase> straight_border;

//! A nullable pointer reference to a \ref straight_border "straight border".

typedef ptr<straight_borderObj, straight_borderBase> straight_borderptr;

//! A reference to a constant \ref straight_border "straight border".

typedef const_ref<straight_borderObj, straight_borderBase> const_straight_border;

//! A nullable pointer reference to a constant \ref straight_border "straight border".

typedef const_ptr<straight_borderObj, straight_borderBase> const_straight_borderptr;

LIBCXXW_NAMESPACE_END

#endif
