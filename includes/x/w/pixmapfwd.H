/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_pixmapfwd_h
#define x_w_pixmapfwd_h

#include <x/w/namespace.H>
#include <x/w/types.H>
#include <x/ptrfwd.H>
#include <unordered_map>
#include <string>

LIBCXXW_NAMESPACE_START

class pixmapObj;

//! A Pixmap object.

//! This is a wrapper for the X protocol's Pixmap object.

typedef ref<pixmapObj> pixmap;

//! A constant pixmap

//! \see pixmap

typedef const_ref<pixmapObj> const_pixmap;

//! A nullable pointer to a pixmap

//! \see pixmap

typedef ptr<pixmapObj> pixmapptr;

//! A nullable pointer to a constant pixmap

//! \see pixmap

typedef const_ptr<pixmapObj> const_pixmapptr;

//! Points of interest in a \ref pixmap "pixmap".

//! A container for points of interest in this pixmap.
//! Generic, opaque container. Loaded with metadata if the pixmap
//! comes from a scalable sxg image.
//!
//! Currently used by sxg images for custom pointer images, to define a
//! "hotspot" for the pointer.

typedef std::unordered_map<std::string, std::pair<coord_t, coord_t>
                          > pixmap_points_of_interest_t;

LIBCXXW_NAMESPACE_END

#endif
