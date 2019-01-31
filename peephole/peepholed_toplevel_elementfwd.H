/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholed_toplevel_elementfwd_h
#define peepholed_toplevel_elementfwd_h

#include "x/w/namespace.H"
#include <x/ptrfwd.H>
#include "peephole/peepholed_toplevelobj.H"

LIBCXXW_NAMESPACE_START

//! A mixin for an element in a peephole for the top level display element.

//! The template parameter must be an elementObj or a subclass of it.
//!
//! Multiply-inherits from the template parameter and a peepholed_toplevelObj.

template<typename element_impl>
class LIBCXX_HIDDEN peepholed_toplevel_elementObj
	: public element_impl,
	  public peepholed_toplevelObj {

public:
	using element_impl::element_impl;

	element get_peepholed_element() override;
};

//! A peepholed element

typedef ref<peepholed_toplevelObj> peepholed_toplevel;

LIBCXXW_NAMESPACE_END

#endif
