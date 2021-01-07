/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholed_toplevelobj_h
#define peepholed_toplevelobj_h

#include "peephole/peepholedobj.H"
#include "peephole/peepholed_toplevelfwd.H"
#include "x/w/screenfwd.H"

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
	//! recalculate_peepholed_metrics() gets invoked before invoking
	//! max_width() and max_height().
	virtual void recalculate_peepholed_metrics(ONLY IN_THREAD,
						   const screen &my_screen)=0;

	//! Return the maximum width of the toplevel element.
	virtual dim_t max_width(ONLY IN_THREAD) const=0;

	//! Return the maximum height of the toplevel element.
	virtual dim_t max_height(ONLY IN_THREAD) const=0;
};

LIBCXXW_NAMESPACE_END

#endif
