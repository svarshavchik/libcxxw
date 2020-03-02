/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef connection_infofwd_h
#define connection_infofwd_h

#include <x/ptrfwd.H>
#include "x/w/namespace.H"

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN connection_infoObj;

typedef ref<connection_infoObj> connection_info;

typedef ptr<connection_infoObj> connection_infoptr;

LIBCXXW_NAMESPACE_END

#endif
