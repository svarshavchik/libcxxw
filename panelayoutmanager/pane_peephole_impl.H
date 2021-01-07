/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef pane_peephole_impl_h
#define pane_peephole_impl_h

#include "panelayoutmanager/pane_peephole.H"
#include "panelayoutmanager/pane_peephole_containerobj.H"
#include "peephole/peephole_impl_elementfwd.H"
#include "x/w/impl/always_visiblefwd.H"
#include "x/w/impl/container_elementfwd.H"
#include "x/w/impl/theme_font_elementfwd.H"
#include "x/w/impl/child_element.H"

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

//! The peephole container for a pane. The actual elements in the container
//! that uses the \ref panelayoutmanager "pane layout manager" are
//! containers with the peephole grid layout manager, which have this
//! peephole container, and its associated scrollbars.
//!
//! This is the container for the peephole with the actual display element.

class LIBCXX_HIDDEN pane_peepholeObj::implObj
	: public theme_font_elementObj<always_visibleObj
					   <peephole_impl_elementObj<
						    container_elementObj<
							    child_elementObj>>
					    >> {

	//! Alias.

	typedef theme_font_elementObj<always_visibleObj
				      <peephole_impl_elementObj<
					      container_elementObj<
						      child_elementObj>>
				       >> superclass_t;
 public:
	//! My parent container.

	const ref<pane_peephole_containerObj::implObj> parent_container;

	//! Constructor
	implObj(const ref<pane_peephole_containerObj::implObj>
		&parent_container,
		const child_element_init_params &init_params);

	//! Destructor
	~implObj();
};

LIBCXXW_NAMESPACE_END

#endif
