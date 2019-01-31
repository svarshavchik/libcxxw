/*
** Copyright 2017-2019 Double Precision, Inc.
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

class LIBCXX_HIDDEN peephole_gridlayoutmanagerObj;

//! Grid layout manager for a peephole (with scrollbars).

//! Overrides request_visibility_recursive().

class peephole_gridlayoutmanagerObj
	: public gridlayoutmanagerObj::implObj {

 public:

	struct LIBCXX_HIDDEN init_args;

	//! Constructor

	//! The elements that go into the grid get pre-created, and we save
	//! them here.
	peephole_gridlayoutmanagerObj(const init_args &args);

	peephole_gridlayoutmanagerObj(const container_impl
				      &container_impl,
				      const peephole &my_peephole,
				      const scrollbar &my_vertical_scrollbar,
				      const scrollbar &my_horizontal_scrollbar);

	//! My peephole
	const peephole my_peephole;

	//! My vertical scrollbar
	const scrollbar my_vertical_scrollbar;

	//! My horizontal scrollbar
	const scrollbar my_horizontal_scrollbar;

	//! Override request_visibility_recursive().

	//! The recursive visiblity change gets propagated only to the
	//! peephole element in cell (0,0). The peephole manages the visibility
	//! of the scrollbars, by itself.

	void request_visibility_recursive(ONLY IN_THREAD, bool) override;
};

//! Constructor parameters.

struct peephole_gridlayoutmanagerObj::init_args {
	const container_impl &my_container_impl;
	const peephole &my_peephole;
	const scrollbar &my_vertical_scrollbar;
	const scrollbar &my_horizontal_scrollbar;
};



LIBCXXW_NAMESPACE_END

#endif
