/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholed_toplevel_element_h
#define peepholed_toplevel_element_h

#include "peephole/peepholed_toplevel_elementfwd.H"
#include "peephole/peepholed_toplevel.H"
#include "x/w/element.H"

LIBCXXW_NAMESPACE_START

//! Inherited from peepholed_toplevelObj.

template<typename element_impl>
element peepholed_toplevel_elementObj<element_impl>::get_peepholed_element()
{
	return element(this);
}

LIBCXXW_NAMESPACE_END

#endif
