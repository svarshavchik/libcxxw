/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef fontpatternfwd_H
#define fontpatternfwd_H

#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

namespace fontconfig {
#if 0
}
#endif

class patternObj;

//! Font pattern.

//! This is a facade for the fontconfig library's FcPattern object: font
//! properties, and information.

typedef ref<patternObj> pattern;

//! A nullable pointer reference to \ref pattern "font properties".

typedef ptr<patternObj> patternptr;

//! Constant \ref pattern "font properties".

typedef const_ref<patternObj> const_pattern;

//! A nullable pointer reference to constant \ref pattern "font properties".

typedef const_ptr<patternObj> const_patternptr;

#if 0
{
#endif
}

LIBCXXW_NAMESPACE_END

#endif
