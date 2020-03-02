/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholed_element_h
#define peepholed_element_h

#include "peephole/peepholed_elementfwd.H"
#include "peephole/peepholed.H"
#include "x/w/element.H"

LIBCXXW_NAMESPACE_START

//! Inherited from peepholedObj.

template<typename element_impl>
element peepholed_elementObj<element_impl>::get_peepholed_element()
{
	return element(this);
}

LIBCXXW_NAMESPACE_END

#endif
