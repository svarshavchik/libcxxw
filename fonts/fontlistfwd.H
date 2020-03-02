/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef fontlistfwd_H
#define fontlistfwd_H

#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

namespace fontconfig {
#if 0
}
#endif

class LIBCXX_HIDDEN listObj;

//! Font list.

//! This is a wrapper for the fontconfig library's FcFontSet object, a list of
//! fonts.

typedef ref<listObj> list;

//! A nullable pointer reference to \ref list "font properties".

typedef ptr<listObj> listptr;

//! Constant \ref list "font properties".

typedef const_ref<listObj> const_list;

//! A nullable pointer reference to constant \ref list "font properties".

typedef const_ptr<listObj> const_listptr;

#if 0
{
#endif
}

LIBCXXW_NAMESPACE_END

#endif
