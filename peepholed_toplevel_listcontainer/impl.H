/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholed_toplevel_listcontainer_impl_h
#define peepholed_toplevel_listcontainer_impl_h

#include "x/w/impl/child_elementfwd.H"
#include "x/w/connection_threadfwd.H"
#include "x/w/types.H"
#include "x/w/impl/child_element.H"
#include "x/w/impl/focus/focusable.H"
#include "listlayoutmanager/list_elementfwd.H"
#include "listlayoutmanager/listcontainer_pseudo_implfwd.H"
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

	tallest_row_height_t tallest_row_height_thread_only={1,1};

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

	virtual bool update_tallest_row_height(ONLY IN_THREAD,
					       const tallest_row_height_t
					       &new_tallest_height);

	//! Return the listcontainer_pseudo_implobj.

	//! Returns what this mixin implementation is a part of.

	virtual listcontainer_pseudo_implObj &listcontainer_element()=0;
};

LIBCXXW_NAMESPACE_END

#endif
