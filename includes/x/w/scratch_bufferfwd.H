/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_scratch_bufferfwd_h
#define x_w_scratch_bufferfwd_h

#include "x/w/namespace.H"
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC scratch_bufferObj;

//! A picture and a pixmap used for preparing display element images.

//! This is returned by screen object's create_scratch_buffer().
//!
//! \internal

typedef ref<scratch_bufferObj> scratch_buffer;

//! A constant \ref scratch_buffer "scratch buffer".

typedef const_ref<scratch_bufferObj> const_scratch_buffer;

//! A nullable pointer reference to a \ref scratch_buffer "scratch buffer".


typedef ptr<scratch_bufferObj> scratch_bufferptr;

//! A nullable pointer reference to a constant \ref scratch_buffer "scratch buffer".

typedef const_ptr<scratch_bufferObj> const_scratch_bufferptr;
LIBCXXW_NAMESPACE_END

#endif
