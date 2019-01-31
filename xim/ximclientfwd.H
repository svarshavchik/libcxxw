/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef ximclientfwd_H
#define ximclientfwd_H

#include <x/ptrfwd.H>
#include "x/w/namespace.H"

#include <cstdint>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN ximclientObj;

//! XIM Input Context ID.

typedef uint16_t xim_ic_t;

/*! An XIM client context.

*/

typedef ref<ximclientObj> ximclient;

//! A nullable pointer reference to a \ref ximclient "XIM client".
typedef ptr<ximclientObj> ximclientptr;

LIBCXXW_NAMESPACE_END

#endif
