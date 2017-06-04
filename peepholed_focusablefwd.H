/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholed_focusablefwd_h
#define peepholed_focusablefwd_h

#include <x/ptrfwd.H>
#include <x/w/namespace.H>

LIBCXXW_NAMESPACE_START

class peepholed_focusableObj;

//! Information about a peepholed focusable.

//! The scrollbars returned by \ref create_focusable_peephole "create_focusable_peephole()"
//! and the focusable display element get used to construct the
//! peephole_focusable object.
//!
//! The public display element object delegates get_impl() and
//! internal_impl_count() methods here.

typedef ref<peepholed_focusableObj> peepholed_focusable;

LIBCXXW_NAMESPACE_END

#endif
