/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef peephole_layoutmanager_impl_H
#define peephole_layoutmanager_impl_H

#include "peephole/peepholeobj.H"
#include "peephole/peephole_style.H"
#include "x/w/impl/layoutmanager.H"
#include "peephole/peepholed_elementfwd.H"
#include "x/w/gridlayoutmanagerfwd.H"
#include "x/w/elementfwd.H"
#include "x/w/gridfactoryfwd.H"
#include "x/w/focusablefwd.H"
#include "x/w/button_eventfwd.H"
#include <x/logger.H>
#include <xcb/xproto.h>

LIBCXXW_NAMESPACE_START

//! \ref peephole "peephole" implementation object.

class LIBCXX_HIDDEN peepholeObj::layoutmanager_implObj
	: public layoutmanagerObj::implObj {


 public:
	LOG_CLASS_SCOPE;

	class scrollbarsObj;

	//! This peephole's style.
	const peephole_style style;

	//! The element in the peephole.

	const peepholed element_in_peephole;

	layoutmanager_implObj(const container_impl &container_impl,
			      peephole_style style,
			      const peepholed &element_in_peephole);

	~layoutmanager_implObj();

	virtual bool process_button_event(ONLY IN_THREAD,
					  const button_event &be,
					  xcb_timestamp_t timestamp);

	//! Implement child_metrics_updated()

	//! My element's metrics must've been updated. Resize the element
	//! according to its preferred size.

	void child_metrics_updated(ONLY IN_THREAD) override;

	//! Implement do_for_each_child().

	//! We always have one child element.
	void do_for_each_child(ONLY IN_THREAD,
			       const function<void (const element &e)> &)
		override;

	//! Implement num_children().
	size_t num_children(ONLY IN_THREAD) override;

	//! Implement create_public_object().

	//! We just construct a base layoutmanager object.
	layoutmanager create_public_object() override;

	//! Implement recalculate().

	void recalculate(ONLY IN_THREAD) override;

	//! Implement ensure_visibility().

	//! We can only be called to ensure peephole_element's visibility.

	void ensure_visibility(ONLY IN_THREAD,
			       elementObj::implObj &e,
			       const rectangle &r) override;

	//! Implement process_updated_position().

	void process_updated_position(ONLY IN_THREAD,
				      const rectangle &position) override;

	//! Horizontal scrollbar was moved.

	void update_horizontal_scroll(ONLY IN_THREAD, dim_t offset);

	//! Vertical scrollbar was moved.

	void update_vertical_scroll(ONLY IN_THREAD, dim_t offset);

	//! Override initialize().
	void initialize(ONLY IN_THREAD) override;
 private:

	//! Implement recalculate()

	//! If a rectangle is provided, make sure client's requested visibility
	//! is honored.

	void recalculate_with_requested_visibility(ONLY IN_THREAD,
						   rectangle *r);

	//! See if we can scroll the element.

	//! Computed the peepholed element's new position. Determine whether
	//! the element can be repositioned by scrolling the visible peephole
	//! using CopyArea, and manually refreshing the exposed areas.
	//!
	//! If all the qualification are met, the scroll gets performed and
	//! \c true gets returned; a \c false return indicates the element's
	//! update_current_position() should be called, and let it handle
	//! everything.

	bool attempt_scroll_to(ONLY IN_THREAD, const rectangle &);

	//! Set our metrics, based on the peepholed element's
	void update_our_metrics(ONLY IN_THREAD);
 protected:
	virtual void update_scrollbars(ONLY IN_THREAD,
				       const rectangle &element_pos,
				       const rectangle &current_position);
};

LIBCXXW_NAMESPACE_END

#endif
