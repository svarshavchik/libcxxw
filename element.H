/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef element_h
#define element_h

#include "x/w/rectangle.H"
#include "x/w/screenfwd.H"
#include "x/w/picturefwd.H"
#include "x/w/generic_windowobj.H"
#include "x/w/element_state.H"
#include "connection_threadfwd.H"
#include "elementfwd.H"
#include "draw_infofwd.H"
#include "picture.H"
#include "metrics_horizvertobj.H"
#include <x/obj.H>
#include <x/callback_list.H>
#include <x/logger.H>

LIBCXXW_NAMESPACE_START

//! Base class for element implementation.

//! Stuff that all display elements have.

class LIBCXX_HIDDEN elementObj::implObj : virtual public obj,

// horizvertObj is considered to be IN_THREAD_ONLY

					  private metrics::horizvertObj {

 protected:
	LOG_CLASS_SCOPE;
 private:

	//! Most of the stuff is for the connection thread's consumption, only.

	struct data_thread_only_t {

		//! Current position
		rectangle current_position;


		////////////////////////////////////////////////////////////
		//
		// Defaulted values.

		//! Requested visibility.

		//! show()/hide() sets this, then adds this element to the
		//! visibility_updated set.

		bool requested_visibility=false;

		//! Actual visibility.

		//! When the connection thread is done with its
		//! work, it goes through all elements in visibility_updated
		//! and if requested_visibility != actual_visibility,
		//! update_visibility gets called.

		bool actual_visibility=false;

		//! Inherited visibility

		//! An element is actually visible only if the element,
		//! and all of its parent elements are visible.

		bool inherited_visibility=false;

		//! State update handlers

		callback_list<void (const element_state &)> update_handlers=
			callback_list<void (const element_state &)>::create();
	};

	//! For the connection thread's use, only.

	data_thread_only_t data_thread_only;

 public:
	//! Return my window

	virtual generic_windowObj::handlerObj &get_window_handler()=0;

	//! Return my window
	virtual const generic_windowObj::handlerObj &get_window_handler() const=0;

	//! Use parent display element's background color.

	virtual void remove_background_color(IN_THREAD_ONLY)=0;

	//! Use its own background color.

	virtual void set_background_color(IN_THREAD_ONLY,
					  const const_picture &)=0;

	//! Return the element's screen
	screen get_screen();

	//! Return the element's screen
	const_screen get_screen() const;

	//! Nesting level.

	//! Top level window elements have a nesting level of 0.
	//! All others have one more than their parent element.
	const size_t nesting_level;

	//! Constructor
	implObj(size_t nesting_level,
		const rectangle &initial_position);

	//! Constructor
	implObj(size_t nesting_level,
		const rectangle &initial_position,
		const metrics::axis &horiz,
		const metrics::axis &vert);

	//! Destructor
	~implObj();

	THREAD_DATA_ONLY(data);
	THREAD_DATA_SUPERCLASS_ONLY(horizvertObj, get_horizvert);
 public:


	//! Install a state update callback.
	template<typename functor_type>
	ref<obj> on_state_update(functor_type &&functor)
	{
		return do_on_state_update(element_state_update_handler_t
					  ::create(std::forward<functor_type>
						   (functor)));
	}

	//! Install a state update callback.
	ref<obj> do_on_state_update(const element_state_update_handler_t &);

	//! Somebody wishes to inform us of our new current_position

	//! If the given rectangle is different than the existing current
	//! position, update it, and call notify_updated_position()
	//! and current_position_updated().

	void update_current_position(IN_THREAD_ONLY,
				     const rectangle &r);

	//! This element's current_position has been updated.

	//! Unsert this element into the connection thread's
	//! element_position_updated list.

	void current_position_updated(IN_THREAD_ONLY);

	//! Invoked by the connection thread.

	//! The default implementation invokes schedule_redraw().
	//!
	//! Overridden by containers, which notify their layout manager,
	//! before invoking notify_updated_position().

	virtual void process_updated_position(IN_THREAD_ONLY);

	//! Calls invoke_element_state_updates().

	void notify_updated_position(IN_THREAD_ONLY);

	//! Create the current element_state for invoking callbacks.
	element_state create_element_state(IN_THREAD_ONLY,
					   element_state::state_update_t);

	//! Invoke all existing state update callbacks, for some reason.

	void invoke_element_state_updates(IN_THREAD_ONLY,
					  element_state::state_update_t);

	/////////////////////////////////////////////////////////////////////
	//

	//! show() or hide() this display element.

	void request_visibility(bool flag);

	//! show() or hide() this display element.

	//! Executed in the connection thread.
	//!
	//! Overridden in generic_window_handler(). The first time a top
	//! level window is made visible, it gets resized to its preferred
	//! width, first.

	virtual void request_visibility(IN_THREAD_ONLY, bool flag);

	//! Recursive show() or hide().

	void request_visibility_recursive(bool flag);

	//! Recursive show() or hide().

	//! Executed in the connection thread.
	//! The default implementation invokes request_visibility().
	//! Overridden by the container implementation object.

	virtual void request_visibility_recursive(IN_THREAD_ONLY, bool flag);

	//! Executed in the connection thread.

	//! If different visibility was updated, invoke update_visibility().

	void update_visibility(IN_THREAD_ONLY);

	//! update_visibility() invokes this callback.

	//! This is overridden in child_elementObj, which also checks
	//! the parent's inherited visibility. If the parent is hidden,
	//! this element is not visible, and flag is forced to off, before
	//! invoking this method.
	//!
	//! visibility_updated() compares flag with the
	//! current value of inherited_visibility flag. If it's different,
	//! inherited_visibility_updated() gets invoked.
	virtual void visibility_updated(IN_THREAD_ONLY, bool flag);


	//! This element's inherited_visibility flag has changed.

	//! This default implementation calls do_inherited_visibility_updated().
	//! If this is a container, this gets overridden, and the container
	//! also updates the containers' elements inherited visibility,
	//! before invoking do_inherited_visibility_updated().

	virtual void inherited_visibility_updated(IN_THREAD_ONLY,
						  bool flag);

	//! Finally update this element's inherited visibility status.

	//! The relationships with the parent element and any child elements
	//! (if this is a container), have been squared away.
	//!
	//! Update the flag, invoke before/after showing/hiding callbacks.

	void do_inherited_visibility_updated(IN_THREAD_ONLY,
					     bool flag);

	//! Actually set the inherited visibility flag, and I do really mean it.

	//! do_inherited_visibility_updated() invokes set_inherited_visibility()
	//! after invoking the before_showing/hiding callbacks, and before
	//! invoking the after_showing/hiding callbacks.
	//!
	//! The default implementation sets data.inherited_visibility.
	//!
	//! Overridden in generic_window_handler to also invoke
	//! xcb_map_window or xcb_unmap_window.
	virtual void set_inherited_visibility(IN_THREAD_ONLY, bool flag);

	//! Redraw this display element, after showing or hiding it.

	//! visibility_updated() invokes it after it invokes
	//! inherited_visibility_updated().
	//!
	//! The default implementation calls schedule_redraw(). Overriden by
	//! generic_windowObj::handlerObj to do nothing. Top level windows are
	//! redrawn by expose events.

	virtual void draw_after_visibility_updated(IN_THREAD_ONLY,
						   bool flag);


	//! Return my viewport.

	//! The element calls this if it wants to draw itself.
	//! It passes its current_position for the initial_viewport.
	//! The implementation may modify it, before returning the final
	//! viewport in get_draw_info().

	virtual draw_info get_draw_info(IN_THREAD_ONLY,
					const rectangle &initial_viewport)=0;

	class clip_region_set;

	//! Prepare draw_info before draw() gets called.

	//! The default implementation does nothing. child_elementObj
	//! overrides and sets the display element's custom background color,
	//! if any.

	virtual void prepare_draw_info(IN_THREAD_ONLY,
				       draw_info &);

	//! If this display element is visible, call schedule_redraw().

	void schedule_redraw_if_visible(IN_THREAD_ONLY);

	//! Schedule the connection thread to invoke explicit_redraw().

	void schedule_redraw(IN_THREAD_ONLY);

	//! Forcibly redraw this display element

	//! If this display element is not visible, calls clear_to_color().
	//! Otherwise calls draw() (which then has to call do_draw(), because
	//! the element is visible).

	void explicit_redraw(IN_THREAD_ONLY);

	//! Draw the display element.

	//! If the display element is not visible, does nothing.
	//!
	//! Otherwise calls do_draw().
	void draw(IN_THREAD_ONLY,
		  const draw_info &di,
		  const rectangle_set &areas);

	//! Draw the display element.

	//! The default implementation invokes clear_to_color().
	//!
	//! The rectangle cooridnates are specified relative to the viewport.

	virtual void do_draw(IN_THREAD_ONLY,
			     const draw_info &di,
			     const rectangle_set &areas);

	//! Clear to background color

	//! Wrapper that acquires a clip_region_set, first.

	void clear_to_color(IN_THREAD_ONLY,
			    const draw_info &di,
			    const rectangle_set &areas);

	//! Clear to background color

	//! Wrapper that acquires a clip_region_set, first.
	//! The rectangles' coordinates are relative to the viewport.

	void clear_to_color(IN_THREAD_ONLY,
			    const clip_region_set &,
			    const draw_info &di,
			    const rectangle_set &areas);
};

LIBCXXW_NAMESPACE_END

#endif
