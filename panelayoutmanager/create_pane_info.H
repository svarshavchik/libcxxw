/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef create_pane_info_h
#define create_pane_info_h

#include "panelayoutmanager/pane_peepholeobj.H"
#include "panelayoutmanager/pane_peephole_containerobj.H"

LIBCXXW_NAMESPACE_START

//! Intermediate containers for elements in the pane.

//! Display elements that get allegedly created by the pane layout manager's
//! factories do not actually end up being created in the pane layout manager's
//! container.
//!
//! The generic factoryObj calls get_container_impl(). get_container_impl()
//! creates a container for the new pane's peephole, and the peephole.
//! The new element gets created in the peephole.
//!
//! When created() gets called we pick up this ball, and run with it.

struct LIBCXX_HIDDEN create_pane_info_t {

	ref<pane_peephole_containerObj
	    ::implObj> peephole_container_impl;
	ref<pane_peepholeObj::implObj> peephole_impl;
};


LIBCXXW_NAMESPACE_END

#endif
