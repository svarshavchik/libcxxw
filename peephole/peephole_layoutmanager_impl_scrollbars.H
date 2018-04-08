/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef peephole_layoutmanager_impl_scrollbars_H
#define peephole_layoutmanager_impl_scrollbars_H

#include "peephole/peephole_layoutmanager_impl.H"
#include "x/w/scrollbar.H"
#include "x/w/rgb.H"

#include <optional>

LIBCXXW_NAMESPACE_START

struct LIBCXX_HIDDEN peephole_scrollbars;

//! Create scrollbar elements for a peephole.

//! This must be called, passing in the scrollbars' container, before
//! constructing the peephole layout manager.
//!
//! The resulting peephole_scrollbars gets passed to peephole layout
//! manager's constructor.

peephole_scrollbars
create_peephole_scrollbars(const ref<containerObj::implObj> &container,
			   const std::optional<color_arg> &background_color)
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

void install_peephole_scrollbars(const gridlayoutmanager &lm,
				 const scrollbar &vertical_scrollbar,
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

//! Set correct focus order for a focusable element in a peephole.

//! A new focusable element has been created, as the last focusable element
//! in the tabbing order. Reorder the top level peephole's scrollbars to
//! follow the new focusable element in the tabbing order, so that the
//! top level peephole's scrollbars get always tabbed to last.

void set_top_level_peephole_scrollbar_focus_order
(ONLY IN_THREAD,
 focusableObj::implObj &new_element,
 const focusable &horizontal_scrollbar,
 const focusable &vertical_scrollbar) LIBCXX_HIDDEN;

//! Add automatic scrollbars to a peephole.

//! A subclass of peepholeObj::layoutmanager_implObj that links the peephole
//! with scrollbars that scroll the peephole automatically, and updates
//! the scrollbar when the peephole gets scrolled on its own.
//!
//! Implements process_button_event(), to scroll the peephole vertically.
//! Implements update_scrollbars() to reflect the position of the scrolled
//! viewport.

class LIBCXX_HIDDEN peepholeObj::layoutmanager_implObj::scrollbarsObj
	: public layoutmanager_implObj {

 public:
	class callbackObj;

 private:
	//! The horizontal scrollbar element
	const scrollbar h_scrollbar;

	//! The vertical scrollbar lement
	const scrollbar v_scrollbar;

	//! Horizontal scrollbar configuration

	scrollbar_visibility horizontal_scrollbar_visibility_thread_only;

	//! Vertical scrollbar configuration

	scrollbar_visibility vertical_scrollbar_visibility_thread_only;
 public:
	THREAD_DATA_ONLY(horizontal_scrollbar_visibility);
	THREAD_DATA_ONLY(vertical_scrollbar_visibility);

	//! An internal implementation object used by the scrollbar callback.
	const ref<callbackObj> h_callback;

	//! An internal implementation object used by the scrollbar callback.
	const ref<callbackObj> v_callback;

	//! Scrollwheel action.

	void vert_scroll_low(ONLY IN_THREAD, const input_mask &);

	//! Scrollwheel action.

	void vert_scroll_high(ONLY IN_THREAD, const input_mask &);

 protected:

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

 public:

	//! Constructor
	scrollbarsObj(const ref<containerObj::implObj> &container_impl,
		      peephole_style style,
		      const peepholed &element_in_peephole,
		      const peephole_scrollbars &scrollbars,
		      const scrollbar_visibility horizontal_scrollbar_visibility,
		      const scrollbar_visibility vertical_scrollbar_visibility);

	//! Finish initialization of the scrollbars

	//! This must be called after this layout manager is constructed
	//! in order to finish the initialization of the scrollbars.

	void initialize_scrollbars();

	//! Destructor
	~scrollbarsObj();

	//! Implement button_event().

	//! Scroll the peephole vertically in response to a thumbwheel click.
	//! Implements button 4 and 5 clicks.

	bool process_button_event(ONLY IN_THREAD,
				  const button_event &be,
				  xcb_timestamp_t timestamp) override;
 private:

	//! Implement update_scrollbars().

	void update_scrollbars(ONLY IN_THREAD,
			       const rectangle &element_pos,
			       const rectangle &current_position) override;

	//! Update the given scrollbar after recalculation.

	void update_scrollbar(ONLY IN_THREAD,
			      const ref<scrollbarObj::implObj> &scrollbar,
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

	//! Alias
	typedef peepholeObj::layoutmanager_implObj::scrollbarsObj scrollbarsObj;

	//! Horizontal scrollbar's callback object.

	const ref<scrollbarsObj::callbackObj> h_callback;

	//! Vertical scrollbar's callback object.

	const ref<scrollbarsObj::callbackObj> v_callback;

	//! Constructor
	peephole_scrollbars(const scrollbar &horizontal_scrollbar,
			    const scrollbar &vertical_scrollbar,
			    const ref<scrollbarsObj::callbackObj> &h_callback,
			    const ref<scrollbarsObj::callbackObj> &v_callback);

	//! Copy constructor
	peephole_scrollbars(const peephole_scrollbars &);

	//! Move constructor
	peephole_scrollbars(peephole_scrollbars &&);

	//! Destructor
	~peephole_scrollbars();
};

LIBCXXW_NAMESPACE_END

#endif
