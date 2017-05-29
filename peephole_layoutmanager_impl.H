/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef peephole_layoutmanager_impl_H
#define peephole_layoutmanager_impl_H

#include "peepholeobj.H"
#include "layoutmanager.H"
#include "scrollbar/scrollbarfwd.H"
#include "peepholed_elementfwd.H"
#include "x/w/elementfwd.H"
#include "x/w/scrollbar.H"
#include "x/w/gridfactoryfwd.H"
#include "x/w/focusablefwd.H"
#include <x/logger.H>

LIBCXXW_NAMESPACE_START

struct LIBCXX_HIDDEN peephole_scrollbars;

//! Create scrollbar elements for a peephole.

//! This must be called, passing in the scrollbars' container, before
//! constructing the peephole layout manager.
//!
//! The resulting peephole_scrollbars gets passed to peephole layout
//! manager's constructor.

peephole_scrollbars
create_peephole_scrollbars(const ref<containerObj::implObj> &container)
	LIBCXX_HIDDEN;

//! Install scrollbar elements for a peephole.

//! Everything is about ready to be wrapped up. The peephole element
//! is installed in the grid layout manager's cell (0, 0). Let's take
//! the scrollbars that create_peephole_scrollbars() created, take their
//! visibility, the factory for the grid layout's row #0 (which
//! already contains the peephole element and the factory is going to
//! install its new element into row 1), and the factory for the
//! grid layout's row #1 (which is empty), and officially install the
//! scrollbars into the grid.

void install_peephole_scrollbars(const scrollbar &vertical_scrollbar,
				 scrollbar_visibility v_visibility,
				 const gridfactory &row0_factory,
				 const scrollbar &horizontal_scrollbar,
				 scrollbar_visibility h_visibility,
				 const gridfactory &row1_factory)
	LIBCXX_HIDDEN;

//! Set correct focus order for a focusable element in a peephole.

//! After everything gets constructed, we'll arrange for the vertical
//! scrollbar to get focus after the focusable element, and the horizontal
//! scrollbar after the vertical scrollbar.

void set_peephole_scrollbar_focus_order(const focusable &element,
					const focusable &horizontal_scrollbar,
					const focusable &vertical_scrollbar)
	LIBCXX_HIDDEN;

//! Set scrollbar focus order.

//! If the peephole is not for a focusable element, the horizontal scrollbar
//! gets focus after the vertical one, that's it.

void set_peephole_scrollbar_focus_order(const focusable &horizontal_scrollbar,
					const focusable &vertical_scrollbar)
	LIBCXX_HIDDEN;

//! \ref peephole "peephole" implementation object.

class LIBCXX_HIDDEN peepholeObj::layoutmanager_implObj
	: public layoutmanagerObj::implObj {

	LOG_CLASS_SCOPE;

 public:
	class scrollbar_implObj;

	//! The element in the peephole.

	const peepholed element_in_peephole;

	//! Finish initialization of the scrollbars

	//! This must be called after this layout manager is constructed
	//! in order to finish the initialization of the scrollbars.

	void initialize_scrollbars();

 private:
	//! Horizontal scrollbar configuration

	scrollbar_visibility horizontal_scrollbar_visibility_thread_only;

	//! Vertical scrollbar configuration

	scrollbar_visibility vertical_scrollbar_visibility_thread_only;
 public:
	THREAD_DATA_ONLY(horizontal_scrollbar_visibility);
	THREAD_DATA_ONLY(vertical_scrollbar_visibility);

	//! The internal implementation object for the horizontal scrollbar
	const ref<scrollbar_implObj> horizontal_scrollbar;

	//! The internal implementation object for the vertical scrollbar
	const ref<scrollbar_implObj> vertical_scrollbar;

 private:

	//! The horizontal scrollbar display element

	//! This is not the horizontal_scrollbar. This is a focusframe around
	//! it. This is the object that should be shown or hidden, depending
	//! on whether the peephole is active or not.
	const elementimpl horizontal_scrollbar_element;

	//! The vertical scrollbar display element

	//! This is not the vertical_scrollbar. This is a focusframe around
	//! it. This is the object that should be shown or hidden, depending
	//! on whether the peephole is active or not.
	const elementimpl vertical_scrollbar_element;

	//! The peephole element's most recent requested visibility.

	rectangle requested_visibility;

 public:

	//! Constructor
	layoutmanager_implObj(const ref<containerObj::implObj> &container_impl,
			      const peepholed &element_in_peephole,
			      const peephole_scrollbars &scrollbars,
			      const scrollbar_visibility horizontal_scrollbar_visibility,
			      const scrollbar_visibility vertical_scrollbar_visibility);

	//! Destructor
	~layoutmanager_implObj();

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

 private:
	//! Implement recalculate()

	//! If 'flag' is set, make sure client's requested visibility is
	//! honored.

	void recalculate_with_requested_visibility(IN_THREAD_ONLY,
						   bool flag);
 public:
	//! Implement process_updated_position().

	void process_updated_position(IN_THREAD_ONLY,
				      const rectangle &position) override;

	//! Implement ensure_visibility().

	//! We can only be called to ensure peephole_element's visibility.

	void ensure_visibility(IN_THREAD_ONLY,
			       elementObj::implObj &e,
			       const rectangle &r) override;

	//! Horizontal scrollbar was moved.

	void update_horizontal_scroll(IN_THREAD_ONLY, dim_t offset);

	//! Vertical scrollbar was moved.

	void update_vertical_scroll(IN_THREAD_ONLY, dim_t offset);

 private:

	//! Update the given scrollbar after recalculation.

	void update_scrollbar(IN_THREAD_ONLY,
			      const ref<scrollbar_implObj> &scrollbar,
			      const elementimpl &visibility_element,
			      const scrollbar_visibility visibility,
			      coord_t pos,
			      dim_t size,
			      dim_t peephole_size,
			      dim_t increment);
};

//! Scrollbar elements for a peephole

//! A temporary object that gets used during creation of a peephole.

struct LIBCXX_HIDDEN peephole_scrollbars {

	//! The horizontal scrollbar

	const scrollbar horizontal_scrollbar;

	//! The vertical scrollbar

	const scrollbar vertical_scrollbar;

	//! Horizontal scrollbar's implementation object.

	const ref<peepholeObj::layoutmanager_implObj
		  ::scrollbar_implObj> horizontal_scrollbar_impl;

	//! Vertical scrollbar's implementation object.

	const ref<peepholeObj::layoutmanager_implObj
		  ::scrollbar_implObj> vertical_scrollbar_impl;

	//! Constructor
	peephole_scrollbars(const scrollbar &horizontal_scrollbar,
			    const scrollbar &vertical_scrollbar,
			    const ref<peepholeObj::layoutmanager_implObj
			    ::scrollbar_implObj> &horizontal_scrollbar_impl,
			    const ref<peepholeObj::layoutmanager_implObj
			    ::scrollbar_implObj> &vertical_scrollbar_impl);

	//! Copy constructor
	peephole_scrollbars(const peephole_scrollbars &);

	//! Move constructor
	peephole_scrollbars(peephole_scrollbars &&);

	//! Destructor
	~peephole_scrollbars();
};

LIBCXXW_NAMESPACE_END

#endif
