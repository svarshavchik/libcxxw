/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef elementfwd_h
#define elementfwd_h

#include <x/ptrfwd.H>
#include "x/w/elementfwd.H"
#include "x/w/elementobj.H"
#include "x/w/namespace.H"

LIBCXXW_NAMESPACE_START

typedef ref<elementObj::implObj> elementimpl;

typedef ptr<elementObj::implObj> elementimplptr;

LIBCXXW_NAMESPACE_END

#endif
