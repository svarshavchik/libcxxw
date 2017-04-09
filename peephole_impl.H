/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef peephole_impl_H
#define peephole_impl_H

#include "peepholeobj.H"
#include "container_elementfwd.H"
#include "child_element.H"

LIBCXXW_NAMESPACE_START

//! \ref peephole "peephole" implementation object.

class LIBCXX_HIDDEN peepholeObj::implObj
	: public container_elementObj<child_elementObj> {

 public:

	using container_elementObj<child_elementObj>::container_elementObj;

	//! Destructor
	~implObj();

	//! Retrieve the element in the peephole.

	template<typename lambda>
		void get_element(lambda &&callback);
};

LIBCXXW_NAMESPACE_END

#endif
