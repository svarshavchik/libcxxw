/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_pane_peepholeobj_h
#define x_w_pane_peepholeobj_h

#include "panelayoutmanager/pane_peepholefwd.H"
#include "x/w/containerobj.H"

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see pane_peephole

class LIBCXX_HIDDEN pane_peepholeObj : public containerObj {

public:
	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	pane_peepholeObj(const ref<implObj> &impl,
			 const layout_impl &container_layout_impl);

	//! Destructor
	~pane_peepholeObj();
};

LIBCXXW_NAMESPACE_END

#endif
