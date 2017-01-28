/*
** Copyright 2014 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_gcfwd_H
#define x_w_gcfwd_H

#include <x/ptrfwd.H>
#include <x/w/namespace.H>

LIBCXXW_NAMESPACE_START

class gcObj;
class gcBase;

//! A graphic context

//! A low-level object used by the library to compose alpha masks in a pixmap.

typedef ref<gcObj, gcBase> gc;

//! A nullable pointer reference to an \ref gc "graphic context"

typedef ptr<gcObj, gcBase> gcptr;

//! A constant \ref gc "graphic context"
typedef const_ref<gcObj, gcBase> const_gc;

//! A nullable pointer reference to a constant \ref gc "graphic context"

typedef const_ptr<gcObj, gcBase> const_gcptr;

LIBCXXW_NAMESPACE_END

#endif
