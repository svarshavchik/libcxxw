/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholed_toplevelobj_h
#define peepholed_toplevelobj_h

#include "peephole/peepholedobj.H"

LIBCXXW_NAMESPACE_START

//! A top level peephole interface.

//! Inherits from peepholedObj(), which specifies the default scrollbar
//! increments, like the regular peephole interface, and adds max_width()
//! and max_height().
//!
//! \see peepholed_toplevel

class LIBCXX_HIDDEN peepholed_toplevelObj : public peepholedObj {

 public:

	//! Constructor
	peepholed_toplevelObj();

	//! Destructor
	~peepholed_toplevelObj();

	//! Recalculate size of the top level peephole

	//! Invoked by the layout manager when either the peephole or its
	//! element changes size, or when the window manager frame extents
	//! have changed.
	//!
	//! recalculate_metrics() gets invoked before invoking max_width()
	//! and max_height().
	virtual void recalculate_metrics(IN_THREAD_ONLY)=0;

	//! Return the maximum width of the toplevel element.
	virtual dim_t max_width(IN_THREAD_ONLY) const=0;

	//! Return the maximum height of the toplevel element.
	virtual dim_t max_height(IN_THREAD_ONLY) const=0;
};

LIBCXXW_NAMESPACE_END

#endif
