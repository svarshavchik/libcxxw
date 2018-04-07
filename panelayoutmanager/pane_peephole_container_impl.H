/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef pane_peephole_container_impl_h
#define pane_peephole_container_impl_h

#include "panelayoutmanager/pane_peephole_container.H"
#include "always_visiblefwd.H"
#include "container_elementfwd.H"
#include "x/w/impl/child_element.H"

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

class LIBCXX_HIDDEN pane_peephole_containerObj::implObj
	: public always_visibleObj<container_elementObj<child_elementObj>> {

	//! Alias.

	typedef always_visibleObj<container_elementObj<child_elementObj>
				  > superclass_t;
 public:
	//! Constructor
	implObj(const ref<containerObj::implObj> &parent_container,
		const child_element_init_params &init_params);

	//! Destructor
	~implObj();
};

LIBCXXW_NAMESPACE_END

#endif
