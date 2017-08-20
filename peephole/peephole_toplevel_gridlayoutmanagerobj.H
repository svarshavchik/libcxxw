/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peephole_toplevel_gridlayoutmanagerobj_h
#define peephole_toplevel_gridlayoutmanagerobj_h

#include "peephole/peephole_gridlayoutmanagerobj.H"

LIBCXXW_NAMESPACE_START

//! Grid layout manager for the top level display element.

//! \see peephole_toplevel_gridlayoutmanager

class LIBCXX_HIDDEN peephole_toplevel_gridlayoutmanagerObj
	: public peephole_gridlayoutmanagerObj {

 public:

	using peephole_gridlayoutmanagerObj::peephole_gridlayoutmanagerObj;

	//! Override needs_recalculation().

	//! Simultaneously trigger recalculation of the peepholed container,
	//! which will recalculate_metrics() of the toplevel peepholed
	//! element.
	void needs_recalculation(IN_THREAD_ONLY) override;
};

LIBCXXW_NAMESPACE_END

#endif
