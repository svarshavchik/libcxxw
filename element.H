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
#include "x/w/pictformatfwd.H"
#include "x/w/scratch_bufferfwd.H"
#include "x/w/input_mask.H"
#include "connection_threadfwd.H"
#include "elementfwd.H"
#include "element_draw.H"
#include "draw_infofwd.H"
#include "draw_info_cachefwd.H"
#include "background_colorfwd.H"
#include "metrics_horizvertobj.H"
#include "focus/focus.H"
#include "fonts/current_fontcollectionfwd.H"
#include "x/w/key_eventfwd.H"
#include "x/w/fontfwd.H"
#include <x/obj.H>
#include <x/callback_list.H>
#include <x/logger.H>
#include <x/functional.H>
#include <experimental/string_view>

LIBCXXW_NAMESPACE_START

class clip_region_set;
class richtextstring;
class richtextmeta;
class text_param;

//! Base class for element implementation.

//! Stuff that all display elements have.

class LIBCXX_HIDDEN elementObj::implObj : public element_drawObj,

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

		//! Whether the element has been removed from its container.

		bool removed=false;

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

	//! Invoke schedule_redraw() if this element is visible.
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

	//! Available scratch buffer, for drawing in this element.

	const scratch_buffer element_scratch_buffer;

	//! Constructor
	implObj(size_t nesting_level,
		const rectangle &initial_position,
		const screen &my_screen,
		const const_pictformat &my_pictformat,
		const std::string &scratch_buffer_id);

	//! Constructor
	implObj(size_t nesting_level,
		const rectangle &initial_position,
		const metrics::horizvert_axi &initial_metrics,
		const screen &my_screen,
		const const_pictformat &my_pictformat,
		const std::string &scratch_buffer_id);

	//! Destructor
	~implObj();

	THREAD_DATA_ONLY(data);
	THREAD_DATA_SUPERCLASS_ONLY(horizvertObj, get_horizvert);

	//! The container has removed this element.

	//! Recursively invokes removed_from_container if this element itself
	//! is a container.
	//!
	//! If data.removed is not set, sets it and invoked removed().

	void removed_from_container(IN_THREAD_ONLY);

	//! This element has been removed from its container just now.

	//! The default implementation does nothing.
	virtual void removed(IN_THREAD_ONLY);

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
						  inherited_visibility_info &info);

	//! Finally update this element's inherited visibility status.

	//! The relationships with the parent element and any child elements
	//! (if this is a container), have been squared away.
	//!
	//! Update the flag, invoke before/after showing/hiding callbacks.

	void do_inherited_visibility_updated(IN_THREAD_ONLY,
					     inherited_visibility_info &info);

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

	virtual void set_inherited_visibility(IN_THREAD_ONLY,
					      inherited_visibility_info
					      &visibility_info);

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

	//! We don't need the full draw_info, just the absolute location.

	virtual rectangle get_absolute_location(IN_THREAD_ONLY)=0;

	//! Prepare draw_info before draw() gets called.

	//! The default implementation does nothing. child_elementObj
	//! overrides and sets the display element's custom background color,
	//! if any.

	virtual void prepare_draw_info(IN_THREAD_ONLY,
				       draw_info &);

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
				      const rectangle_set &areas);

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


	//! Implementation of draw_using_scratch_buffer() with element_scratch_uffer.
	void do_draw_using_scratch_buffer(IN_THREAD_ONLY,
					  const function
					  <scratch_buffer_draw_func_t> &cb,
					  const rectangle &rect,
					  const draw_info &di,
					  const draw_info &background_color_di,
					  const clip_region_set &clipped)
		override;

	//! Implementation of draw_using_scratch_buffer() with a custom scratch buffer.
	void do_draw_using_scratch_buffer(IN_THREAD_ONLY,
					  const function
					  <scratch_buffer_draw_func_t> &cb,
					  const rectangle &rect,
					  const draw_info &di,
					  const draw_info &background_color_di,
					  const clip_region_set &clipped,
					  const scratch_buffer &buffer)
		override;

	//! A subclass drew all or a portion of this element.

	//! Composite it into draw_info.window_picture. All drawing of this
	//! element's contents is ultimately funneled through draw_to_window
	//! picture.

	void draw_to_window_picture(IN_THREAD_ONLY,
				    //! The draw operation is now clipped
				    const clip_region_set &set,

				    //! This element's draw_info
				    const draw_info &di,

				    //! Where the subclass drew what it drew.
				    const const_picture &contents,

				    //! Which portion of this element is drawn.

				    //! The subclass draws a portion of the
				    //! this element specified by "rect"
				    //! in "contents".
				    const rectangle &rect);

	//! Clear to background color

	//! Wrapper that acquires a clip_region_set, first.

	void clear_to_color(IN_THREAD_ONLY,
			    const draw_info &di,
			    const rectangle_set &areas);

	//! Clear to background color

	//! The rectangles' coordinates are relative to the viewport.
	//! background_color_di is the draw_info of the element whose background
	//! color is drawn here (default is the same as di).
	void clear_to_color(IN_THREAD_ONLY,
			    const clip_region_set &,
			    const draw_info &di,
			    const draw_info &background_color_di,
			    const rectangle_set &areas);

	//! A new theme has been installed.

	//! This is invoked from the window handler's theme_updated_event(),
	//! Invoke schedule_redraw(), then recursively invoke
	//! theme_updated() (as long as we're visible, of course).

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

	////////////////////////////////////////////////////////////////////
	//
	// Fonts, colors, text.

	//! Create a font for this display element.

	//! Creates a non-standard font. This gets forwarded to
	//! \ref generic_windowObj::handlerObj "window handler's"
	//! create_font().

	current_fontcollection create_font(const font &props);

	//! Create a theme font for a display element in this window.

	//! Creates a theme font. This gets forwarded to
	//! \ref generic_windowObj::handlerObj "window handler's"
	//! create_theme_font().

	current_fontcollection create_theme_font(const std::experimental::string_view &font);

	//! Shortcut for invoking the same screen method

	background_color create_background_color(const std::experimental
						 ::string_view &color_name,
						 const rgb &default_value);

	//! Create a \ref richtextstring "rich text string", based on a \ref text_param "text_param" and a default \ref richtextmeta "richtextmeta".

	//! Converts between the library API version of a rich text string,
	//! and the internal object.
	//!
	//! font specifies the initial font and color, if not explicitly
	//! specified in the text_param.

	richtextstring convert(richtextmeta font,
			       const text_param &t);


	//////////////////////////////////////////////////////////////////
	//
	// Input focus handling

	//! Used in input focus processing, accessed by the connection thread only.

	bool original_focus;

	//! Used in input focus processing, accessed by the connection thread only.

	bool new_focus;

	//! The focus reporter parameter to request_focus() and leave_focus().

	typedef void (elementObj::implObj::*focus_reporter_t)
		(IN_THREAD_ONLY, focus_change, const ref<elementObj::implObj> &)
		;

	//! Input focus is being requested for this display element.

	//! Generates calls to <type>_focus() for all affected display elements.
	//!
	//! focus_reporter parameter is either &report_keyboard_focus or
	//! &report_pointer_focus.
	//!
	//! \see focus_change

	void request_focus(IN_THREAD_ONLY,
			   const ptr<elementObj::implObj> &current_focus,
			   focus_reporter_t focus_reporter);

	//! Input focus is being lost completely.

	//! focus_reporter parameter is either &report_keyboard_focus or
	//! &report_pointer_focus.
	//!
	//! Generates calls to <type>_focus() for all affected display elements.
	//! \see focus_change

	void lose_focus(IN_THREAD_ONLY,
			focus_reporter_t focus_reporter)
	{
		requested_focus_from(IN_THREAD);
		leaving_focus(IN_THREAD, ptr<elementObj::implObj>(),
			      focus_reporter);
	}

	//! Internal focus processing, see element_focusable.C

	virtual void requested_focus_to(IN_THREAD_ONLY,
					const ptr<elementObj::implObj> &current_focus);

	//! Internal focus processing, see element_focusable.C
	virtual void requested_focus_from(IN_THREAD_ONLY);

	//! Internal focus processing, see element_focusable.C

	void leaving_focus(IN_THREAD_ONLY,
			   const ptr<elementObj::implObj> &leaving_for,
			   focus_reporter_t focus_reporter);

	//! Internal focus processing, see element_focusable.C
	virtual void do_leaving_focus(IN_THREAD_ONLY,
				      focus_change &event,
				      const ref<implObj> &focus_from,
				      const ptr<elementObj::implObj>
				      &leaving_for,
				      focus_reporter_t reporter);

	//! Internal focus processing, see element_focusable.C

	void entering_focus(IN_THREAD_ONLY,
			    const ptr<elementObj::implObj> &focus_from,
			    focus_reporter_t focus_reporter);

	//! Internal focus processing, see element_focusable.C

	virtual void do_entering_focus(IN_THREAD_ONLY,
				       focus_change event,
				       const ref<implObj> &focus_to,
				       const ptr<elementObj::implObj> &focus_from,
				       focus_reporter_t focus_reporter);

	//! Invoke the keyboard_focus() virtual function.

	inline void report_keyboard_focus(IN_THREAD_ONLY,
					  focus_change event,
					  const ref<implObj> &ptr)
	{
		keyboard_focus(IN_THREAD, event, ptr);
	}

	//! The result of all this is a bunch of calls to keyboard_focus().

	//! The default implementation does nothing.

	virtual void keyboard_focus(IN_THREAD_ONLY,
				    focus_change event,
				    const ref<implObj> &ptr);

	//! Here come the keypresses.

	//! Returns true or false, indicating whether the key press or
	//! release was consumed.
	//!
	//! The default implementation always returns false.

	virtual bool process_key_event(IN_THREAD_ONLY, const key_event &ke);

	//! Here come the buttons.

	//! Returns true or false, indicating when the button press or
	//! release was consumed.
	//!
	//! The button event is always preceded by a motion event, reporting
	//! the pointer's x & y coordinates.
	//!
	//! The default implementation always returns false.

	virtual bool process_button_event(IN_THREAD_ONLY,
					  int button,
					  bool press,
					  const input_mask &mask);

	//! process_button_event() calls grab() to grab the pointer.

	virtual void grab(IN_THREAD_ONLY)=0;

	//! Invoke the pointer_focus() virtual function.

	inline void report_pointer_focus(IN_THREAD_ONLY,
					  focus_change event,
					  const ref<implObj> &ptr)
	{
		pointer_focus(IN_THREAD, event, ptr);
	}

	//! The result of all this is a bunch of calls to pointer_focus().

	//! The default implementation does nothing.

	virtual void pointer_focus(IN_THREAD_ONLY,
				   focus_change event,
				   const ref<implObj> &ptr);

	//! The pointer has moved.

	//! The default implementation does nothing.
	virtual void motion_event(IN_THREAD_ONLY, coord_t x, coord_t y,
				  const input_mask &mask);
 private:

	//! The most recent keyboard_focus() reported
	focus_change most_recent_keyboard_focus_change_thread_only=focus_change::lost;

	//! The most recent pointer_focus() reported
	focus_change most_recent_pointer_focus_change_thread_only=focus_change::lost;

 public:

	THREAD_DATA_ONLY(most_recent_keyboard_focus_change);
	THREAD_DATA_ONLY(most_recent_pointer_focus_change);

	//! Whether this display element currently has keyboard input focus.

	//! Returns true if most_recent_keyboard_focus_change was one of:
	//! focus_gained, child_gained, child_moved_to, gained_from_child,
	//! lost_to_child.

	bool current_keyboard_focus(IN_THREAD_ONLY);

	//! Whether this display element currently has pointer input focus.

	//! Returns true if most_recent_pointer_focus_change was one of:
	//! focus_gained, child_gained, child_moved_to, gained_from_child,
	//! lost_to_child.

	bool current_pointer_focus(IN_THREAD_ONLY);

	//! Make sure that the given rectangle is visible.

	//! The default implementation does nothing.
	//!
	//! child_elementObj forwards this call to its container.
	//!
	//! The container's layout manager, by default, adds this element's
	//! current_position, and forward it to the container's element.
	//!
	//! The \ref peepholeObj::layoutmanager_implObj "peephole layout manager"
	//! makes sure that its child element's specified position will be
	//! scrolled into view.

	virtual void ensure_visibility(IN_THREAD_ONLY, const rectangle &);

};

LIBCXXW_NAMESPACE_END

#endif
