/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef recycled_pixmapsfwd_h
#define recycled_pixmapsfwd_h

#include "x/w/namespace.H"
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN recycled_pixmapsObj;

//! A container for recycled pixmaps and pictures.

//! An object held by each screen that contains weak containers of
//! cached and recycled images, consisting of pixmaps and their pictures.
//!
//! A small collection of screen methods return a pixmap and a picture, for
//! a given set of descriptive identifiers, such as a pictformat, and
//! a label. These objects are held in a weak container, in the recycled
//! pixmap object, and an existing object gets returned, if there's a
//! a weakly-referenced one with a matching set of identifiers.
//!
//! In this manner, a display element may request a particular pixmap and
//! a picture for its own use, and end up sharing it with out display
//! elements.

typedef ref<recycled_pixmapsObj> recycled_pixmaps;

//! A constant \ref recycled_pixmaps "recycled pixmap".

typedef const_ref<recycled_pixmapsObj> const_recycled_pixmaps;

//! A nullable pointer reference to a \ref recycled_pixmaps "recycled pixmap".


typedef ptr<recycled_pixmapsObj> recycled_pixmapsptr;

//! A nullable pointer reference to a constant \ref recycled_pixmaps "recycled pixmap".

typedef const_ptr<recycled_pixmapsObj> const_recycled_pixmapsptr;
LIBCXXW_NAMESPACE_END

#endif
