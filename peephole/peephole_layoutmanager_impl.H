/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef peephole_layoutmanager_impl_H
#define peephole_layoutmanager_impl_H

#include "peephole/peepholeobj.H"
#include "peephole/peephole_style.H"
#include "layoutmanager.H"
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

	layoutmanager_implObj(const ref<containerObj::implObj> &container_impl,
			      peephole_style style,
			      const peepholed &element_in_peephole);

	~layoutmanager_implObj();

	virtual bool process_button_event(IN_THREAD_ONLY,
					  const button_event &be,
					  xcb_timestamp_t timestamp);

	//! Implement child_metrics_updated()

	//! My element's metrics must've been updated. Resize the element
	//! according to its preferred size.

	void child_metrics_updated(IN_THREAD_ONLY) override;

	//! Implement do_for_each_child().

	//! We always have one child element.
	void do_for_each_child(IN_THREAD_ONLY,
			       const function<void (const element &e)> &)
		override;

	//! Implement create_public_object().

	//! We just construct a base layoutmanager object.
	layoutmanager create_public_object() override;

	//! Implement recalculate().

	void recalculate(IN_THREAD_ONLY) override;

	//! Implement ensure_visibility().

	//! We can only be called to ensure peephole_element's visibility.

	void ensure_visibility(IN_THREAD_ONLY,
			       elementObj::implObj &e,
			       const rectangle &r) override;

	//! Implement process_updated_position().

	void process_updated_position(IN_THREAD_ONLY,
				      const rectangle &position) override;

	//! Horizontal scrollbar was moved.

	void update_horizontal_scroll(IN_THREAD_ONLY, dim_t offset);

	//! Vertical scrollbar was moved.

	void update_vertical_scroll(IN_THREAD_ONLY, dim_t offset);

	//! Override initialize().
	void initialize(IN_THREAD_ONLY) override;
 protected:

	//! The peephole element's most recent requested visibility.

	rectangle requested_visibility;

	//! Implement recalculate()

	//! If 'flag' is set, make sure client's requested visibility is
	//! honored.

	void recalculate_with_requested_visibility(IN_THREAD_ONLY,
						   bool flag);

	virtual void update_scrollbars(IN_THREAD_ONLY,
				       const rectangle &element_pos,
				       const rectangle &current_position);
};

LIBCXXW_NAMESPACE_END

#endif
