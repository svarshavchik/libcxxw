/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef element_h
#define element_h

#include "x/w/rectangle.H"
#include "x/w/screenfwd.H"
#include "x/w/generic_windowobj.H"
#include "x/w/element_state.H"
#include "connection_threadfwd.H"
#include "elementfwd.H"
#include "draw_infofwd.H"
#include "draw_info_cachefwd.H"
#include "background_colorfwd.H"
#include "metrics_horizvertobj.H"
#include <x/obj.H>
#include <x/callback_list.H>
#include <x/logger.H>
#include <x/functional.H>

LIBCXXW_NAMESPACE_START

//! Base class for element implementation.

//! Stuff that all display elements have.

class LIBCXX_HIDDEN elementObj::implObj : virtual public obj,

// horizvertObj is considered to be IN_THREAD_ONLY
//
// **** use get_horizvert() to access it. ****
					  private metrics::horizvertObj {

 protected:
	LOG_CLASS_SCOPE;
 private:

	//! Most of the stuff is for the connection thread's consumption, only.

	struct data_thread_only_t {

		//! Current position

		//! The X & Y coordinates are relative to parent's

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

		//! Whether initialize() has been called.

		bool initialized=false;

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

	//! Schedules the virtual method to be invoked by the connection thread.

	void remove_background_color();

	//! Use parent display element's background color.

	virtual void remove_background_color(IN_THREAD_ONLY)=0;


	//! Use a standard background color for this display element.

	//! If there's no background color of the given name, use the
	//! default value.

	void set_background_color(const std::experimental::string_view &name,
				  const rgb &default_value);

	//! Use a custom background color for this display element.

	void set_background_color(const const_picture &background_color);

	//! Set the background color.

	//! Schedules the virtual method to be invoked by the connection thread.

	void set_background_color(const background_color &);

	//! Install the background color

	virtual void set_background_color(IN_THREAD_ONLY,
					  const background_color &)=0;

	//! Whether this element uses its own custom background color

	//! The default implementation always returns true.
	//! child_element overrides this, and returns the correct values.

	virtual bool has_own_background_color(IN_THREAD_ONLY);

	//! This element's background color has changed.

	//! Invoke schedule_redraw_if_visible(), in order to redraw it.
	//! But wait, there's more!. If this is a container, and the
	//! container has any visible child elements that use the parent's
	//! background color, they need to be redrawn too!

	void background_color_changed(IN_THREAD_ONLY);

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
		const metrics::horizvert_axi &initial_metrics);

	//! Destructor
	~implObj();

	THREAD_DATA_ONLY(data);
	THREAD_DATA_SUPERCLASS_ONLY(horizvertObj, get_horizvert);

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

	//! Invoke schedule_update_position_processing(), then recursively
	//! invoke current_position_updated() for any and all child elements.
	//! The net effective is that this element, and its child elements,
	//! are scheduled to be redrawn.
	//!
	//! generic_window_handler overrides this. A redraw is not required
	//! when the top level window's position changed. If its size also
	//! changed, and the top level window's layout manager has to reposition
	//! the child elements, that will itself trigger a redraw.

	virtual void current_position_updated(IN_THREAD_ONLY);

	//! Add this element to the element_position_updated() queue.

	//! Adds this element to the queue that the connection thread uses
	//! to invoke process_updated_position().
	//!
	//! generic_window_handler invokes this directly, bypassing
	//! current_position_updated(). It is not necessary to recursively
	//! do this for all display elements when the top level window is
	//! resized or moved. It is only necessary to schedule the top level
	//! window to execute current_position_updated(), as the top level
	//! container will recalculate and reposition all elements, as needed.

	void schedule_update_position_processing(IN_THREAD_ONLY);

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
	//! Invokes schedule_update_visibility().
	//!
	//! Overridden in generic_window_handler(). The first time a top
	//! level window is made visible, it gets resized to its preferred
	//! width, first.

	virtual void request_visibility(IN_THREAD_ONLY, bool flag);

	//! Schedule update_visibility() to be invoke by the connection thread.

	//! Inserts this element into the visibility_update list.

	void schedule_update_visibility(IN_THREAD_ONLY);

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
	//!
	//! Also overridden in child_elementObj to also invoke its container's
	//! child_background_color_changed() callback, since when the
	//! child element is not shown, it falls back to using the parent's
	//! background color, so officially its background color has changed.

	virtual void set_inherited_visibility(IN_THREAD_ONLY, bool flag);

	//! Redraw this display element, after showing or hiding it.

	//! visibility_updated() invokes it after it invokes
	//! inherited_visibility_updated().
	//!
	//! The default implementation calls schedule_redraw(). Overridden
	//! by containers to draw their elements after they become visible.
	//! The containers draw their own area by overriding do_draw().

	virtual void draw_after_visibility_updated(IN_THREAD_ONLY,
						   bool flag);


	//! Calculate draw_info for this element.

	//! If it's not cached already, calculate it and cache, return a
	//! reference to the cached draw_info.

	virtual draw_info &get_draw_info(IN_THREAD_ONLY)=0;

	//! Clip drawing operation to draw_info.element_viewport.

	//! This install a clip mask into di.window_picture that clips
	//! any drawinfo operation (to the di.absolute_location region)
	//! into the element_viewport.

	class clip_region_set {

	public:
		//! Constructor - install clip mask for drawing.

		clip_region_set(IN_THREAD_ONLY,
				implObj &me,
				const draw_info &di);
	};

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

	void explicit_redraw(IN_THREAD_ONLY, draw_info_cache &);

	//! Exposure event handling.

	//! We get areas to redraw for this element. Invoke draw(),
	//! then recursively invoke ourselves for all visible children,
	//! after calculating each child's rectangle_set consisting of the
	//! given rectangle_set intersected with the child's viewport.

	void exposure_event_recursive(IN_THREAD_ONLY,
				      rectangle_set &areas);

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

	//! A new theme has been installed.

	//! This is invoked from the window handler's theme_updated_event(),
	//! Invoke schedule_redraw_if_visible(), then recursively invoke
	//! theme_updated() for all visible elements.

	virtual void theme_updated(IN_THREAD_ONLY);

	//! Invoke initialize(), if necessary.

	inline void initialize_if_needed(IN_THREAD_ONLY)
	{
		auto &flag=data(IN_THREAD).initialized;

		if (flag)
			return; // Already initialized

		flag=true;

		// update_visibility() bailed out if we were not initialized(),
		// so reschedule this.

		schedule_update_visibility(IN_THREAD);
		initialize(IN_THREAD);
	}

	//! Initialize this display element.

	//! After a display element gets added to a container, the layout
	//! manager should invoke initialize() from the connection thread.
	//!
	//! The default implementation does nothing.

	virtual void initialize(IN_THREAD_ONLY);

	//! Whether this display element has been initialized

	inline bool initialized(IN_THREAD_ONLY) const
	{
		return data(IN_THREAD).initialized;
	}

	//! Invoke a lambda for each child element.

	template<typename functor_type>
		void for_each_child(IN_THREAD_ONLY,
				    functor_type &&functor)
	{
		do_for_each_child(IN_THREAD,
				  make_function<void (const element &e)>
				  (std::forward<functor_type>(functor)));
	}

	//! Iterate over all child elements.

	//! The default implementation does nothing. This is overridden in
	//! containerObj.

	virtual void do_for_each_child(IN_THREAD_ONLY,
				       const function<void
				       (const element &e)> &);

};

LIBCXXW_NAMESPACE_END

#endif
