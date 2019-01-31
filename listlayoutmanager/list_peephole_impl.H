/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef list_peephole_impl_H
#define list_peephole_impl_H

#include "peephole/peephole_impl.H"

LIBCXXW_NAMESPACE_START

//! The peephole for the selection list.

class LIBCXX_HIDDEN list_peephole_implObj : public peepholeObj::implObj {

 public:

	using peepholeObj::implObj::implObj;

	//! Destructor
	~list_peephole_implObj();
};

LIBCXXW_NAMESPACE_END

#endif
