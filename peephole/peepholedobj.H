/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholedobj_h
#define peepholedobj_h

#include "x/w/types.H"
#include "x/w/elementfwd.H"
#include "peephole/peepholedfwd.H"
#include "connection_threadfwd.H"

#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! An element in a peephole.

class LIBCXX_HIDDEN peepholedObj : virtual public obj {

 public:

	//! Constructor
	peepholedObj();

	//! Destructor
	~peepholedObj();

	//! Return the peepholed element.

	virtual element get_peepholed_element()=0;

	//! Return the preferred horizontal increment
	virtual dim_t horizontal_increment(ONLY IN_THREAD) const=0;

	//! Return the preferred vertical increment
	virtual dim_t vertical_increment(ONLY IN_THREAD) const=0;
};

LIBCXXW_NAMESPACE_END

#endif
