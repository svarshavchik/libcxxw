/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholed_elementobj_h
#define peepholed_elementobj_h

#include "x/w/elementobj.H"
#include "x/w/types.H"
#include "peepholed_elementfwd.H"
#include "connection_threadfwd.H"

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN peepholed_elementObj : public elementObj {

 public:
	using elementObj::elementObj;

	~peepholed_elementObj();

	//! Return the preferred horizontal increment
	virtual dim_t horizontal_increment(IN_THREAD_ONLY) const=0;

	//! Return the preferred vertical increment
	virtual dim_t vertical_increment(IN_THREAD_ONLY) const=0;
};

LIBCXXW_NAMESPACE_END

#endif
