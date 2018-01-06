/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_pane_peephole_containerobj_h
#define x_w_pane_peephole_containerobj_h

#include "panelayoutmanager/pane_peephole_containerfwd.H"
#include "x/w/focusable_containerobj.H"
#include "x/w/focusablefwd.H"
#include "peephole/peepholefwd.H"
#include <x/mpobj.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see pane_peephole_container

class LIBCXX_HIDDEN pane_peephole_containerObj :
	public focusable_containerObj {

public:
	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	pane_peephole_containerObj(const ref<implObj> &impl,
				   const ref<layoutmanagerObj::implObj>
				   &layout_impl);

	//! Destructor
	~pane_peephole_containerObj();

	//! Implement get_impl

	ref<focusableImplObj> get_impl() const override;

	//! Implement do_get_impl()
	void do_get_impl(const function<internal_focusable_cb> &cb)
		const override;

	//! Return the peephole container.
	peephole get_peephole();

	//! The focusable object in this pane, if it's a focusable object.

	mutable mpobj<focusableptr> focusable_element;
};

LIBCXXW_NAMESPACE_END

#endif
