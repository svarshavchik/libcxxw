/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peephole_gridlayoutmanagerobj_h
#define peephole_gridlayoutmanagerobj_h

#include "gridlayoutmanager.H"
#include "x/w/containerfwd.H"
#include "x/w/scrollbarfwd.H"
#include "peephole/peepholefwd.H"
#include "x/w/connection_threadfwd.H"

LIBCXXW_NAMESPACE_START

//! Grid layout manager for a peephole (with scrollbars).

//! Overrides request_visibility_recursive().

class LIBCXX_HIDDEN peephole_gridlayoutmanagerObj
	: public gridlayoutmanagerObj::implObj {

 public:

	//! Constructor
	peephole_gridlayoutmanagerObj(const ref<containerObj::implObj>
				      &container_impl);

	//! Return the peephole container.

	//! It's in cell (0, 0)

	peephole get_peephole();

	//! Return the vertical scrollbar

	//! It's in cell (0, 1)

	scrollbar get_vertical_scrollbar();

	//! Return the horizontal_scrollbar

	//! It's in cell(1, 0)

	scrollbar get_horizontal_scrollbar();

	//! Override request_visibility_recursive().

	//! The recursive visiblity change gets propagated only to the
	//! peephole element in cell (0,0). The peephole manages the visibility
	//! of the scrollbars, by itself.

	void request_visibility_recursive(ONLY IN_THREAD, bool) override;
};

LIBCXXW_NAMESPACE_END

#endif
