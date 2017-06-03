/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholed_toplevelobj_h
#define peepholed_toplevelobj_h

#include "peepholedobj.H"

LIBCXXW_NAMESPACE_START

//! A top level peephole interface.

class LIBCXX_HIDDEN peepholed_toplevelObj : public peepholedObj {

 public:

	//! Constructor
	peepholed_toplevelObj();

	//! Destructor
	~peepholed_toplevelObj();

	//! Return the maximum width of the toplevel element.
	virtual dim_t max_width(IN_THREAD_ONLY) const=0;

	//! Return the maximum height of the toplevel element.
	virtual dim_t max_height(IN_THREAD_ONLY) const=0;
};

LIBCXXW_NAMESPACE_END

#endif
