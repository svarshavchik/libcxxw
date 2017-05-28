/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef peephole_get_element_H
#define peephole_get_element_H

#include "peephole_impl.H"
#include "peephole_layoutmanager_impl.H"

LIBCXXW_NAMESPACE_START

//! Retrieve the element in the peephole.

template<typename lambda>
void peepholeObj::implObj::get_element(lambda &&callback)
{
	this->invoke_layoutmanager
		([&]
		 (const ref<peepholeObj::layoutmanager_implObj> &l)
		 {
			 callback(l->element_in_peephole);
		 });
}

LIBCXXW_NAMESPACE_END

#endif
