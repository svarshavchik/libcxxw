/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef fontsortedlistfwd_H
#define fontsortedlistfwd_H

#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

namespace fontconfig {
#if 0
}
#endif

class LIBCXX_HIDDEN sortedlistObj;

//! A sorted list of fonts.

//! A sorted list of fonts that best matches a pattern. Returned by
//! \ref fontpattern "fontpattern"'s match().

typedef ref<sortedlistObj> sortedlist;

//! A nullable pointer reference to \ref sortedlist "sorted fonts".

typedef ptr<sortedlistObj> sortedlistptr;

//! Constant \ref sortedlist "sorted fonts".

typedef const_ref<sortedlistObj> const_sortedlist;

//! A nullable pointer reference to constant \ref sortedlist "sorted fonts".

typedef const_ptr<sortedlistObj> const_sortedlistptr;

#if 0
{
#endif
}

LIBCXXW_NAMESPACE_END

#endif
