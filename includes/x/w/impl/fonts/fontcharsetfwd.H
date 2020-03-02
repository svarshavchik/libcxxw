/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_impl_fonts_fontcharsetfwd_H
#define x_w_impl_fonts_fontcharsetfwd_H

#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

namespace fontconfig {
#if 0
}
#endif

class LIBCXX_HIDDEN charsetObj;

//! Font object set

//! This is a handle for the internal FcCharset object used by the
//! fontconfig library.

typedef ref<charsetObj> charset;

//! A nullable pointer reference to the \ref charset "object set".

typedef ptr<charsetObj> charsetptr;

//! A constant \ref charset "object set".

typedef const_ref<charsetObj> const_charset;

//! A nullable pointer reference to a constant \ref charset "object set".

typedef const_ptr<charsetObj> const_charsetptr;

#if 0
{
#endif
}

LIBCXXW_NAMESPACE_END

#endif
