/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholed_toplevel_listcontainer_impl_h
#define peepholed_toplevel_listcontainer_impl_h

#include "child_elementfwd.H"
#include "connection_threadfwd.H"
#include "x/w/types.H"
#include "child_element.H"
#include "focus/focusable.H"
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! The implementation object for a list container in a top level peephole.

//! A mixin that's combined with a child_elementObj container that uses
//! the peepholed_listcontainer_layoutmanager.
//!
//! This is the companion internal implementation object for a
//! \ref peepholed_toplevel_listcontainer_elementObj "peepholed_toplevel_listcontainer_elementObj".

class LIBCXX_HIDDEN peepholed_toplevel_listcontainer_implObj
	: virtual public obj {

	//! Current tallest row height

	dim_t tallest_row_height_thread_only=1;

 public:
	THREAD_DATA_ONLY(tallest_row_height);

	//! Constructor
	peepholed_toplevel_listcontainer_implObj();

	//! Destructor
	~peepholed_toplevel_listcontainer_implObj();

	//! The layout manager captures the tallest row's height, and calls me.

	//! Returns \c false if the tallest_row_height is unchanged.
	//! Returns \c true if the tallest_row_height has changed. Calls
	//! listcontainer_element's container's needs_recalculation().

	virtual bool update_tallest_row_height(IN_THREAD_ONLY,
					       dim_t new_tallest_height);

	//! Return the child_elementObj this mixin implementation is a part of.

	virtual child_elementObj &listcontainer_element()=0;
};

LIBCXXW_NAMESPACE_END

#endif
