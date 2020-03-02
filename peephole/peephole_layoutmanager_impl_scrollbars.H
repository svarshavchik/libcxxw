/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef peephole_layoutmanager_impl_scrollbars_H
#define peephole_layoutmanager_impl_scrollbars_H

#include "peephole/peephole_layoutmanager_impl.H"
#include "peephole/peephole.H"
#include "peephole/peephole_impl.H"
#include "peephole/peephole_gridlayoutmanagerobj.H"
#include "x/w/scrollbar.H"
#include "x/w/rgb.H"
#include "x/w/border_arg.H"
#include "x/w/alignment.H"
#include "x/w/dim_arg.H"
#include "x/w/scrollbar_appearancefwd.H"

#include <optional>
#include <x/functional.H>

LIBCXXW_NAMESPACE_START

struct LIBCXX_HIDDEN peephole_scrollbars;
struct LIBCXX_HIDDEN peephole_element_factory_ret_t;
struct LIBCXX_HIDDEN peephole_with_scrollbars_info;
struct LIBCXX_HIDDEN create_peephole_with_scrollbars_ret_t;

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

//! A subclass of peepholelayoutmanagerObj::implObj that links the peephole
//! with scrollbars that scroll the peephole automatically, and updates
//! the scrollbar when the peephole gets scrolled on its own.
//!
//! Implements process_button_event(), to scroll the peephole vertically.
//! Implements update_scrollbars() to reflect the position of the scrolled
//! viewport.

class LIBCXX_HIDDEN peepholelayoutmanagerObj::implObj::scrollbarsObj
	: public peepholelayoutmanagerObj::implObj {

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
	const element_impl horizontal_scrollbar_element;

	//! The vertical scrollbar display element

	//! This is not the vertical_scrollbar. This is a focusframe around
	//! it. This is the object that should be shown or hidden, depending
	//! on whether the peephole is active or not.
	const element_impl vertical_scrollbar_element;

 public:

	//! Constructor
	scrollbarsObj(const peephole_with_scrollbars_info &info,
		      const peephole_scrollbars &peephole_scrollbars,
		      // Can be ref<peepholeObj::implObj>:
		      const container_impl &peephole_impl,
		      const peepholed &element_in_peephole);

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
			      const element_impl &visibility_element,
			      const scrollbar_visibility visibility,
			      coord_t pos,
			      dim_t size,
			      dim_t peephole_size,
			      dim_t increment);
};

//! Scrollbar elements for a peephole

//! A temporary object that gets used during creation of a peephole.
//!
//! \see create_peephole_with_scrollbars()

struct peephole_scrollbars {

	//! The horizontal scrollbar

	const scrollbar horizontal_scrollbar;

	//! The vertical scrollbar

	const scrollbar vertical_scrollbar;

	//! Alias
	typedef peepholelayoutmanagerObj::implObj::scrollbarsObj scrollbarsObj;

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


//////////////////////////////////////////////////////////////////////////////

/*!

Peephole scroll-bar parameters for create_peephole_with_scrollbars()

*/
struct peephole_with_scrollbars_info {

	//! The grid container implementation object.
	container_impl grid_container_impl;

	//! Optional non-default background color for the two scroll-bars.
	std::optional<color_arg> background_color;

	//! Peephole style and options
	const peephole_style &style;

	//! Horizontal scroll-bar visibility
	scrollbar_visibility horizontal_visibility;

	//! Vertical scroll-bar visibility
	scrollbar_visibility vertical_visibility;

	//! Horizontal scrollbar appearance
	const const_scrollbar_appearance &horizontal_appearance;

	//! Horizontal scrollbar appearance
	const const_scrollbar_appearance &vertical_appearance;
};

//! Return values from create_peephole_with_scrollbars()

struct create_peephole_with_scrollbars_ret_t {

	//! The new peephole layout manager
	ref<peepholelayoutmanagerObj::implObj> layout_impl;

	//! The grid layout manager implementation object, for the container.
	ref<peephole_gridlayoutmanagerObj> grid_layout_impl;

	//! The grid layout manager.
	gridlayoutmanager grid_layout;
};

//! The peephole element factory callback signature

//! \see create_peephole_with_scrollbars()
typedef peephole_element_factory_ret_t
peephole_element_factory(const ref<peepholelayoutmanagerObj::implObj> &);

//! The peephole layout manager factory callback signature

//! \see create_peephole_with_scrollbars()
typedef ref<peepholelayoutmanagerObj::implObj::scrollbarsObj>
peephole_with_scrollbars_layoutmanager_factory
(const peephole_with_scrollbars_info &info,
 const peephole_scrollbars &scrollbars);

//! The grid layout manager factory callback signature

//! \see create_peephole_with_scrollbars()

typedef ref<peephole_gridlayoutmanagerObj>
peephole_with_scrollbars_gridlayoutmanager_factory
(const peephole_gridlayoutmanagerObj::init_args &init_args);

/*! Return values from the peephole element factory callback.

\see create_peephole_with_scrollbars()
 */
struct peephole_element_factory_ret_t {

	//! The new peephole element
	peephole peephole_container;

	/*! The element that goes into the grid container

	  This is usually the same object as the peephole_container.
	  create_peepholed_focusable_with_frame() creates an intermediate
	  container
	  that uses the border layout manager to implement a focus frame
	  around the peephole, and the peephole_container is inside the
	  border container.
	 */
	element grid_peephole_element;

	//! Optional border around the grid_peephole_element
	std::optional<border_arg> grid_peephole_element_border;

	//! Optional border around the scroll-bars.

	//! Should be set only if grid_peephole_element_border is also set,
	//! and it's the same border.
	std::optional<border_arg> grid_scrollbar_border;

	//! The focusable element inside the peephole

	//! Used by create_peepholed_focus_with_frame().
	//! create_peephole_with_scrollbars() initializes the focus tabbing
	//! order of the scrollbars together with the focusable_peephole_element
	//! if it's specified.
	focusableptr focusable_peephole_element;

	//! grid_peephole_element attribute.

	//! create_peephole_with_scrollbars() sets this for the new
	//! grid_peephole_element when it gets placed into the grid container.
	dim_arg peephole_left_padding=0;

	//! grid_peephole_element attribute.

	//! create_peephole_with_scrollbars() sets this for the new
	//! grid_peephole_element when it gets placed into the grid container.

	dim_arg peephole_right_padding=0;

	//! grid_peephole_element attribute.

	//! create_peephole_with_scrollbars() sets this for the new
	//! grid_peephole_element when it gets placed into the grid container.

	dim_arg peephole_top_padding=0;

	//! grid_peephole_element attribute.

	//! create_peephole_with_scrollbars() sets this for the new
	//! grid_peephole_element when it gets placed into the grid container.

	dim_arg peephole_bottom_padding=0;
};

/*!

Framework for setting up a peephole with scroll-bars

This is done by using a container with a grid layout manager to position
the peephole and both scroll-bars.

Prerequisites:

- A container implementation object for these display elements, the grid
container implementation object.

- The future peephole container implementation object. It should use the
peephole_impl_elementObj mixing, and its parent container implementation
object is the grid container implementation object. It's possible to have
another container have the grid container as its parent container, and the
peephole container's parent is the intermediate container. This is used with
individual focusable elements. The intermediate container uses the border
layout manager to draw a focus frame around the peephole.

- The display element in the container, fully constructed with an
implementation object, whose parent object is the peephole container
implementation object, and its public object that uses the peepholed_elementObj
mixin.

create_peephole_with_scrollbars() takes three callbacks, and a fixed parameter.

- The first callback gets invoked with two parameters: a
peephole_with_scrollbars_info, and a peephole_scrollbars. The callback
is expected to construct a new peepholeObj::layoutmanager_implobj::scrollbarsObj
object and return it. The two parameters get forwarded to scrollbarsObj's
constructor together with peepholed container implementation object and the
peepholed object.

- The second callback receives the scrollbarsObj returned by the first
callback. It is expected to construct the peephole container implementation
object's public object, and return a
\ref peephole_element_factory_ret_t "peephole_element_factory_ret_t" object.

- The third callback gets invoked to create the peephole_gridlayoutmanagerObj,
for the grid container implementation object, and
\ref create_peephole_gridlayoutmanager "create_peephole_gridlayoutmanager"
is the default value for this callback.

The last parameter is a peephole_with_scrollbars_info, and
create_peephole_with_scrollbars() returns a
create_peephole_with_scrollbars_ret_t.

*/

template<typename peephole_with_scrollbars_layoutmanager_factory_t,
	 typename peephole_element_factory_t,
	 typename peephole_with_scrollbars_gridlayoutmanager_factory_t> inline
create_peephole_with_scrollbars_ret_t
create_peephole_with_scrollbars(peephole_with_scrollbars_layoutmanager_factory_t
				&&lm_factory,
				peephole_element_factory_t &&pe_factory,
				peephole_with_scrollbars_gridlayoutmanager_factory_t
				&&glm_factory,
				const peephole_with_scrollbars_info &info)
{
	return do_create_peephole_with_scrollbars
		(make_function<peephole_with_scrollbars_layoutmanager_factory>
		 (std::forward<peephole_with_scrollbars_layoutmanager_factory_t>
		  (lm_factory)),
		 make_function<peephole_element_factory>
		 (std::forward<peephole_element_factory_t>(pe_factory)),
		 make_function<peephole_with_scrollbars_gridlayoutmanager_factory>
		 (std::forward<peephole_with_scrollbars_gridlayoutmanager_factory_t>
		  (glm_factory)),
		 info);
}

create_peephole_with_scrollbars_ret_t do_create_peephole_with_scrollbars
(const function<peephole_with_scrollbars_layoutmanager_factory> &lm_factory,
 const function<peephole_element_factory> &pe_factory,
 const function<peephole_with_scrollbars_gridlayoutmanager_factory> &glm_factory,
 const peephole_with_scrollbars_info &info) LIBCXX_HIDDEN;

ref<peephole_gridlayoutmanagerObj>
create_peephole_gridlayoutmanager(const peephole_gridlayoutmanagerObj
				  ::init_args &init_args) LIBCXX_HIDDEN;

LIBCXXW_NAMESPACE_END

#endif
