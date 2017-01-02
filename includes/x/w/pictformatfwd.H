/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_pictformatfwd_H
#define x_w_pictformatfwd_H

#include <x/ptrfwd.H>
#include <x/w/namespace.H>

LIBCXXW_NAMESPACE_START

class pictformatObj;

//! Available picture formats on the server

typedef ref<pictformatObj> pictformat;

//! A nullable pointer reference to a \ref pictformat "pictformat" to the server

typedef ptr<pictformatObj> pictformatptr;

//! A constant \ref pictformat "pictformat" to the server
typedef const_ref<pictformatObj> const_pictformat;

//! A nullable pointer reference to a constant \ref pictformat "pictformat" to the server

typedef const_ptr<pictformatObj> const_pictformatptr;

LIBCXXW_NAMESPACE_END

#endif
