/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef fontobjectsetfwd_H
#define fontobjectsetfwd_H

#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

namespace fontconfig {
#if 0
}
#endif

class LIBCXX_HIDDEN objectsetObj;

//! Font object set

//! This is a wrapper for the internal FcObjectSet object used by the
//! fontconfig library.

typedef ref<objectsetObj> objectset;

//! A nullable pointer reference to the \ref objectset "object set".

typedef ptr<objectsetObj> objectsetptr;

//! A constant \ref objectset "object set".

typedef const_ref<objectsetObj> const_objectset;

//! A nullable pointer reference to a constant \ref objectset "object set".

typedef const_ptr<objectsetObj> const_objectsetptr;

#if 0
{
#endif
}

LIBCXXW_NAMESPACE_END

#endif
