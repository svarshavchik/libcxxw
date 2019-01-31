/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholed_elementfwd_h
#define peepholed_elementfwd_h

#include "x/w/namespace.H"
#include <x/ptrfwd.H>
#include "peephole/peepholedobj.H"

LIBCXXW_NAMESPACE_START

//! A mixin for an element in a peephole.

//! The template parameter must be an elementObj or a subclass of it.
//!
//! Multiply-inherits from the template parameter and a peepholedObj.
//! Implements get_peepholed_element().

template<typename element_impl>
class LIBCXX_HIDDEN peepholed_elementObj : public element_impl,
					   public peepholedObj {

public:
	using element_impl::element_impl;

	element get_peepholed_element() override;
};

//! A peepholed element

typedef ref<peepholedObj> peepholed;

LIBCXXW_NAMESPACE_END

#endif
