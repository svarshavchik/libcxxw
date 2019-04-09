/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_pane_peephole_containerobj_h
#define x_w_pane_peephole_containerobj_h

#include "panelayoutmanager/pane_peephole_containerfwd.H"
#include "x/w/focusable_containerobj.H"
#include "x/w/focusablefwd.H"
#include "x/w/impl/connection_threadfwd.H"
#include "peephole/peepholefwd.H"
#include <x/mpobj.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see pane_peephole_container

class LIBCXX_HIDDEN pane_peephole_containerObj :
	public focusable_containerObj {

	//! The reference size for this pane.

	//! panelayoutmanagerObj::implObj's reference_size_set flag indicates
	//! whether all panes' reference sizes have been captured.
	//!
	//! The flag is cleared whenever panes are added or removed from the
	//! container, or when the panes size change via sliding.
	//!
	//! When the pane container gets resized, the reference sizes are used
	//! to proportionately scale all panes to adjust the sum total of the
	//! panes and the sliders to match the pane's new size.

	dim_t reference_size_thread_only;

public:
	THREAD_DATA_ONLY(reference_size);

	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	pane_peephole_containerObj(const ref<implObj> &impl,
				   const layout_impl &container_layout_impl);

	//! Destructor
	~pane_peephole_containerObj();

	//! Implement get_impl

	focusable_impl get_impl() const override;

	//! Implement do_get_impl()
	void do_get_impl(const function<internal_focusable_cb> &cb)
		const override;

	//! Return the peephole container.
	peephole get_peephole();

	//! The focusable object in this pane, if it's a focusable object.

	mutable mpobj<focusableptr> focusable_element;

	//! Most recently set peephole size.

	//! Peephole gets resized IN_THREAD. This gets saved here, so that
	//! the current pane metrics can be saved and restore.
	mpobj<dim_t> peephole_size;
};

LIBCXXW_NAMESPACE_END

#endif
