/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_textlistfwd_h
#define x_w_textlistfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class textlistObj;

/*! The singleton element in the textlistlayoutmanager.


*/

typedef ref<textlistObj> textlist;

//! A constant \ref textlist "text list element".

//! \see textlist

typedef const_ref<textlistObj> const_textlist;

//! A nullable pointer reference to a \ref textlist "text list element".

//! \see textlist

typedef ptr<textlistObj> textlistptr;

//! A nullable pointer reference to a const \ref textlist "text list element".

//! \see textlist

typedef const_ptr<textlistObj> const_textlistptr;

LIBCXXW_NAMESPACE_END

#endif
