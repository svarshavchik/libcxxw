/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef list_peepholed_impl_H
#define list_peepholed_impl_H

#include "container_elementfwd.H"
#include "focus/focusable_elementfwd.H"
#include "peephole/peepholed_elementfwd.H"

#include "child_element.H"

LIBCXXW_NAMESPACE_START

//! A container with a list layout manager, in the peephole.

class LIBCXX_HIDDEN list_peepholed_implObj
	: public peepholed_elementObj<focusable_elementObj<container_elementObj<child_elementObj>>> {


 public:

	using peepholeObj::implObj::implObj;

	//! Destructor
	~list_peephole_implObj();
};

LIBCXXW_NAMESPACE_END

#endif
