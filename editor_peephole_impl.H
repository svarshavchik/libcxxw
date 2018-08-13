/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef editor_peephole_impl_H
#define editor_peephole_impl_H

#include "peephole/peephole_impl.H"
#include "x/w/button_eventfwd.H"
#include "editorobj.H"

LIBCXXW_NAMESPACE_START

//! Subclass of peepholeObj::implObj for a peephole that contains an \ref editor "editor".

//! Sets the peephole's metrics based on the configured size of the internal
//! editor element, defined as rows and columns.
//!
//! Also handles button events. editorObj::implObj::process_button_event()
//! does not consume button 1 and button 2 events, so they propagate here.
//! We process these events by setting the focus to the editor element.

class LIBCXX_HIDDEN editor_peephole_implObj
	: public peepholeObj::implObj {

 public:

	using peepholeObj::implObj::implObj;

	//! Button 1 sets focus to the element.

	bool process_button_event(ONLY IN_THREAD,
				  const button_event &be,
				  xcb_timestamp_t timestamp) override;

	//! Forward find_acceptable_drop() to the element

	//! The editor element may not be entirely in the peephole, but we
	//! want to have the edit element to entertain drops with the
	//! dragging located anywhere inside the peephole.

	bool find_acceptable_drop(ONLY IN_THREAD,
				  element_impl &accepting_element,
				  const source_dnd_formats_t &source_formats,
				  xcb_timestamp_t timestamp) override;

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
	void recalculate(ONLY IN_THREAD, editorObj::implObj &e);

	//! Destructor
	~editor_peephole_implObj();
};

LIBCXXW_NAMESPACE_END

#endif
