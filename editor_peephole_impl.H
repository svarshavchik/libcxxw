/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef editor_peephole_impl_H
#define editor_peephole_impl_H

#include "peephole/peephole_impl.H"
#include "x/w/button_eventfwd.H"

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

	//! We now have everything that's needed to set our metrics.

	//! Invoke recalculate().
	void initialize(IN_THREAD_ONLY) override;

	//! Invoke recalculate(), after the theme has been updated().
	void theme_updated(IN_THREAD_ONLY) override;

	//! Button 1 sets focus to the element.

	bool process_button_event(IN_THREAD_ONLY,
				  const button_event &be,
				  xcb_timestamp_t timestamp);
 private:

	//! Compute updated metrics.
	void recalculate(IN_THREAD_ONLY);
 public:

	//! Destructor
	~editor_peephole_implObj();
};

LIBCXXW_NAMESPACE_END

#endif
