/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_pixmapfwd_h
#define x_w_pixmapfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class pixmapObj;

//! A pixmap

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

LIBCXXW_NAMESPACE_END

#endif
