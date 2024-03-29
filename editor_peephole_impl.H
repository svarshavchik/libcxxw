/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef editor_peephole_impl_H
#define editor_peephole_impl_H

#include "peephole/peephole_impl_element.H"
#include "x/w/button_eventfwd.H"
#include "x/w/impl/container_elementfwd.H"
#include "x/w/impl/child_element.H"
#include "editorobj.H"

LIBCXXW_NAMESPACE_START

//! Subclass of peepholeObj::implObj for a peephole that contains an \ref editor "editor".

//! Sets the peephole's metrics based on the configured size of the internal
//! editor element, defined as rows and columns.
//!
//! Also handles button events. editor_implObj::process_button_event()
//! does not consume button 1 and button 2 events, so they propagate here.
//! We process these events by setting the focus to the editor element.

class LIBCXX_HIDDEN editor_peephole_implObj
	: public peephole_impl_elementObj<container_elementObj<child_elementObj>
					  > {
 public:

	typedef peephole_impl_elementObj<container_elementObj<child_elementObj>
					 > superclass_t;

	using superclass_t::superclass_t;

	//! Override report_motion_event().

	//! Remove the cursor pointer on any motion event. The key event
	//! in editor_implObj installs an invisible cursor pointer, in order
	//! to hide it. And we remove it.
	//!
	//! Its our job to remove the cursor pointer because the pointer may
	//! not actually be on top of the editor implementation objcet, which
	//! is a glorified label object that may not cover the entire peephole.

	void report_motion_event(ONLY IN_THREAD,
				 const motion_event &me) override;

	//! Compute updated metrics.

	//! Invoke from the editor element, when its metrics have changed.
	//! We might need to adjust our own metrics as well.
	void recalculate(ONLY IN_THREAD, editor_implObj &e);

	//! Destructor
	~editor_peephole_implObj();
};

LIBCXXW_NAMESPACE_END

#endif
