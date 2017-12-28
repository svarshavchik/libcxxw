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
#include "x/w/button_eventfwd.H"
#include "x/w/motion_eventfwd.H"
#include "x/w/busyfwd.H"
#include "x/w/focus.H"
#include "x/w/rgbfwd.H"
#include "connection_threadfwd.H"
#include "defaultthemefwd.H"
#include "elementfwd.H"
#include "element_draw.H"
#include "draw_info.H"
#include "background_colorfwd.H"
#include "grabbed_pointerfwd.H"
#include "cursor_pointerfwd.H"
#include "metrics_horizvertobj.H"
#include "focus/label_forfwd.H"
#include "focus/label_forobj.H"
#include "fonts/current_fontcollectionfwd.H"
#include "popup/popupfwd.H"
#include "x/w/key_eventfwd.H"
#include "x/w/fontfwd.H"
#include "x/w/tooltipfwd.H"
#include <x/obj.H>
#include <x/logger.H>
#include <x/functional.H>
#include <string_view>
#include <xcb/xproto.h>

#include <functional>
#include <chrono>

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

 public:
	LOG_CLASS_SCOPE;
 private:

	//! Most of the stuff is for the connection thread's consumption, only.

	struct data_thread_only_t {

		//! Current position

		//! The X & Y coordinates are relative to parent's

		rectangle current_position;

		//! Cached draw_info

		//! get_draw_info() retrieves the cached draw_info, or
		//! calls get_draw_info_from_scratch().
		//!
		//! invalidate_cached_draw_info() gets called whenever
		//! the cached draw info may no longer be valid for this
		//! element.

		std::optional<draw_info> cached_draw_info;

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

		//! Whether this element is "enabled".

		//! If this is an element that receives pointer and keyboard
		//! focus, it can be selectively disabled. If this element
		//! is disabled, we draw a shade over its contents.
		//!
		//! The enabled() method checks not only this element's
		//! enabled flag, but also all of its parent containers.
		//! If any of them are disabled this element is considered to
		//! be disabled. The enabled flag only specifies whether
		//! this element is explicitly set as enabled or not.

		bool enabled=true;

		//! State update callback
		std::function<element_state_callback_t> element_state_callback=
			no_element_state_callback;

		//! Pointer focus callback

		std::function<focus_callback_t> on_pointer_callback=
			no_focus_callback;

		//! Keyboard focus callback

		std::function<focus_callback_t> on_keyboard_callback=
			no_focus_callback;

		//! Default pointer and keyboard callback, does nothing.

		static void no_focus_callback(focus_change);

		//! Default element state callback, does nothing.

		static void no_element_state_callback(const element_state &,
						     const busy &);

		//! Callback for reporting key events.

		std::function<key_event_callback_t> on_key_event_callback=
			no_key_event_callback;

		//! Default key event callback, does nothing.

		static bool no_key_event_callback(const all_key_events_t &,
						  bool,
						  const busy &);


		//! Whether this element is a label for some focusable.

		label_forptr label_for;

		//! Most recently reported motion

		coord_t last_motion_x=0;

		//! Most recently reported motion

		coord_t last_motion_y=0;

		//! This element's current cursor pointer, if there is one.
		cursor_pointerptr pointer;

		//! This element's tooltip factory
		std::function<void (const tooltip_factory &)> tooltip_factory;

		//! This element's created tooltip
		popupptr tooltip;

		//! There's a popup attached to this element.
		ptr<implObj> attached_popup;

		//! Scheduled tooltip creation
		tick_clock_t::time_point hover_scheduled_creation;

		//! Timer for the scheduled tooltip creation
		ptr<obj> hover_scheduled_mcguffin;
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

	void set_background_color(const color_arg &theme_color);

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

	//! Install minimum element size overrides.
	void set_minimum_override(dim_t horiz_override, dim_t vert_override);

	//! Install minimum element size overrides.
	virtual void set_minimum_override(IN_THREAD_ONLY,
					  dim_t horiz_override,
					  dim_t vert_override);

	//! The container has removed this element.

	//! Invoked from elementObj's destructor.
	//! Runs removed_from_container() IN_THREAD.

	void removed_from_container();

	//! Set by remove_from_container(), which is only called in destructor.

	//! Prevents the overhead from scheduling the message to the connection
	//! thread twice, because removed_from_container() is also called
	//! in connectionObj's destructor.

	bool removed_from_container_was_called_in_destructor=false;

	//! The container has removed this element.

	//! Executed in the connection thread.
	//! Recursively invokes removed_from_container if this element itself
	//! is a container.
	//!
	//! If data.removed is not set, sets it and invoked removed().

	void removed_from_container(IN_THREAD_ONLY);

	//! This element has been removed from its container just now.

	//! The default implementation does nothing.
	virtual void removed(IN_THREAD_ONLY);

	//! Install a state update callback.
	void on_state_update(const std::function<element_state_callback_t> &cb);

	//! Somebody wishes to inform us of our new current_position

	//! Invoked by a layout manager, to update one of it's elements'
	//! position.
	//!
	//! If the given rectangle is different than the existing current
	//! position, update it, and call notify_updated_position()
	//! and current_position_updated().

	virtual void update_current_position(IN_THREAD_ONLY,
					     const rectangle &r);

	//! The top level window's position changed, but not its size

	//! So nothing has to be recalculated, however popups need to know
	//! about this.
	//!
	//! Propagate absolute_location_updated() to all elements.
	virtual void absolute_location_updated(IN_THREAD_ONLY);

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

	//! Show or hide() this display element.

	void toggle_visibility();

	//! show() or hide() this display element.

	//! Executed in the connection thread.
	//!
	//! Invokes schedule_update_visibility().
	//!
	//! Overridden in generic_window_handler(). The first time a top
	//! level window is made visible, it gets resized to its preferred
	//! width, first.

	virtual void request_visibility(IN_THREAD_ONLY, bool flag);

	//! show() or hide() this display element.

	//! Executed in the connection thread.
	void toggle_visibility(IN_THREAD_ONLY);

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

	//! Ditto

	//! Implement set_inherited_visibility flag.
	virtual void set_inherited_visibility_flag(IN_THREAD_ONLY, bool);

	//! Redraw this display element, after showing or hiding it.

	//! visibility_updated() invokes it after it invokes
	//! inherited_visibility_updated().
	//!
	//! The default implementation calls schedule_redraw(). Overridden
	//! by containers to draw their elements after they become visible.
	//! The containers draw their own area by overriding do_draw().
	//!
	//! Overridden by child_element, calling the container's
	//! child_visibility_updated().

	virtual void draw_after_visibility_updated(IN_THREAD_ONLY,
						   bool flag);


	//! Return the cached_draw_info for this element.

	//! Call get_draw_info_from_scratch() if it does not exist.

	virtual draw_info &get_draw_info(IN_THREAD_ONLY)=0;

	//! Calculate draw_info for this element.

	virtual draw_info &get_draw_info_from_scratch(IN_THREAD_ONLY)=0;

	//! Why invalidate_cached_draw_info() was called.

	enum class draw_info_invalidation_reason {

		//! Something that factored into cached_draw_info for this
		//! element might've changed.
		something_changed,

		//! Something definitely changed, remove any cached_draw_info.
		recursive_invalidation
	};

	//! The cached_draw_info is no longer valid.

	//! Something that factors into draw_info's contents has changed,
	//! so if it is cached make sure it is still valid.

	void invalidate_cached_draw_info(IN_THREAD_ONLY,
					 draw_info_invalidation_reason);

	//! We don't need the full draw_info, just the absolute location.

	virtual rectangle get_absolute_location(IN_THREAD_ONLY)=0;

	//! We need get_absolute_location() for the screen!
	rectangle get_absolute_location_on_screen(IN_THREAD_ONLY);

	//! Schedule the connection thread to invoke explicit_redraw().

	void schedule_redraw(IN_THREAD_ONLY);

	//! schedule_redraw() this one, and all child elements.

	void schedule_redraw_recursively();

	//! schedule_redraw() this one, and all child elements.

	void schedule_redraw_recursively(IN_THREAD_ONLY);

	//! Whether this element is scheduled for redrawing

	bool redraw_scheduled(IN_THREAD_ONLY);

	//! Forcibly redraw this display element

	//! If this display element is not visible, calls clear_to_color().
	//! Otherwise calls draw() (which then has to call do_draw(), because
	//! the element is visible).

	void explicit_redraw(IN_THREAD_ONLY);

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

				    //! draw_to_window_picture() may make
				    //! further modifications.
				    const picture &contents,

				    //! Which portion of this element is drawn.

				    //! The subclass draws a portion of the
				    //! this element specified by "rect"
				    //! in "contents".
				    const rectangle &rect);

	//! Whether draw_to_window_picture() should apply the disabled_mask

	//! Determines whether this display element should be shown as
	//! "disabled".
	//!
	//! Overridden in child_elementObj. Elements in a focusable label
	//! forward this to their container, in order to be drawn as disabled
	//! if their parent container is a label for a focusable.

	virtual bool draw_to_window_picture_as_disabled(IN_THREAD_ONLY);

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

	virtual void theme_updated(IN_THREAD_ONLY,
				   const defaulttheme &new_theme);

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
		initialize_or_log_exception(IN_THREAD);
	}

	//! Return myself.

	//! This allows mixin templates to be added to a subclass of either
	//! elementObj::implObj, or layoutmanagerObj::implObj

	inline auto &get_element_impl()
	{
		return *this;
	}

 private:
	//! Invoke initialize()

	void initialize_or_log_exception(IN_THREAD_ONLY);

 protected:
	//! Initialize this display element.

	//! After a display element gets added to a container, the layout
	//! manager should invoke initialize() from the connection thread.

	virtual void initialize(IN_THREAD_ONLY);

	//! Whether this display element has been initialized

	inline bool initialized(IN_THREAD_ONLY) const
	{
		return data(IN_THREAD).initialized;
	}
 public:
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


	//! Theme font for labels in this container.

	//! The default implementation returns "label".
	//!
	//! Overridden in child_elementObj and forwarded to the container.

	virtual const char *label_theme_font() const;

	//! Foreground color for labels in this container.

	//! The default implementation returns "label_foreground_color".
	//!
	//! Overridden in child_elementObj and forwarded to the container.

	virtual color_arg label_theme_color() const;

	//! Create a font for this display element.

	//! Creates a non-standard font. This gets forwarded to
	//! \ref generic_windowObj::handlerObj "window handler's"
	//! create_font().

	current_fontcollection create_font(const font &props);

	//! Create a theme font for a display element in this window.

	//! Creates a theme font. This gets forwarded to
	//! \ref generic_windowObj::handlerObj "window handler's"
	//! create_theme_font().

	current_fontcollection create_theme_font(const std::string_view &font);

	//! Shortcut for invoking the same screen method

	background_color create_background_color(const color_arg &color_name);

	//! Create a \ref richtextstring "rich text string", based on a \ref text_param "text_param" and a default \ref richtextmeta "richtextmeta".

	//! Converts between the library API version of a rich text string,
	//! and the internal object.
	//!
	//! font specifies the initial font and color, if not explicitly
	//! specified in the text_param.
	//!
	//! Unless allow_links is true, a specified text link throws an
	//! exception.
	richtextstring create_richtextstring(const richtextmeta &default_font,
					     const text_param &t,
					     bool allow_links=false);


	//////////////////////////////////////////////////////////////////
	//
	// Input focus handling

	//! A new focusable element was initialized.

	//! A new focusable element always gets created as the last element
	//! in the tabbing order.
	//!
	//! Overriden by child_elementObj, forwarding this call to its
	//! parent element.
	//! Overriden in peephole_toplevel, to enforce that the top level
	//! peephole's scrollbars are always last in the tabbing order,
	//! repositioning this one's.
	//!
	//! Overriden in generic_windowObj::handlerObj, which ends up
	//! handling either top level windows without peepholes (tooltips),
	//! or the scrollbars themselves.
	virtual void focusable_initialized(IN_THREAD_ONLY,
					   focusableImplObj &fimpl)=0;

	//! This element should be the first focusable element.
	//! child_elementObj forwards it to its parent container; and this
	//! is implemented in generic_windowObj::handlerObj.

	virtual void get_focus_first(IN_THREAD_ONLY, const focusable &)=0;

	//! Whether this element is enabled for focus processing.

	//! Overridden in child_elementObj, to forward to the parent element.
	//! Turning off focus in any parent element automatically turns off
	//! focus processing for all of their child elements, in this manner.

	virtual bool enabled(IN_THREAD_ONLY);

	//! Used in input focus processing, accessed by the connection thread only.

	bool original_focus;

	//! Used in input focus processing, accessed by the connection thread only.

	bool new_focus;

	//! The focus reporter parameter to request_focus() and leave_focus().

	typedef void (elementObj::implObj::*focus_reporter_t)
		(IN_THREAD_ONLY, focus_change);

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
	//!
	//! In testfocusable.C this is equivalent to the (!t.to) case in
	//! testfocusable().

	void lose_focus(IN_THREAD_ONLY,
			focus_reporter_t focus_reporter)
	{
		requested_focus_from(IN_THREAD);
		leaving_focus(IN_THREAD, ptr<elementObj::implObj>(),
			      focus_reporter);
		focus_movement_complete(IN_THREAD, false, focus_reporter);
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

	//! Internal focus processing, see element_focusable.C
	virtual void focus_movement_complete(IN_THREAD_ONLY,
					     bool stop_at_original_focus,
					     focus_reporter_t focus_reporter);

	//! Invoke the keyboard_focus() virtual function.

	void report_keyboard_focus(IN_THREAD_ONLY,
				   focus_change event);

	//! The result of all this is a bunch of calls to keyboard_focus().

	//! The default implementation invokes the keyboard focus callback.

	virtual void keyboard_focus(IN_THREAD_ONLY);

	//! Install a new keyboard focus callback
	void on_keyboard_focus(const std::function<focus_callback_t> &callback);

	//! Install a new keyboard focus callback
	void on_keyboard_focus(IN_THREAD_ONLY,
			       const std::function<focus_callback_t> &callback);

	//! Install a new key event callback

	void on_key_event(const std::function<key_event_callback_t> &);

	//! Install a new key event callback

	void on_key_event(IN_THREAD_ONLY,
			  const std::function<key_event_callback_t> &);


 private:
	//! Invoke the callback
	void invoke_keyboard_focus_callback(IN_THREAD_ONLY);
 public:
	//! Here come the keypresses.

	//! Returns true or false, indicating whether the key press or
	//! release was consumed.
	//!
	//! The default implementation invokes the installed callback.

	virtual bool process_key_event(IN_THREAD_ONLY, const key_event &ke);

	//! When keyboard-bound action should be taken.

	//! On a key press, or key release.
	//! TODO: make it theme-based

	bool activate_for(const key_event &ke) const;

	//! When button-bound action should be taken.

	//! On a key press, or key release.
	//! TODO: make it theme-based

	bool activate_for(const button_event &ke) const;

	//! Here come the buttons.

	//! Returns true or false, indicating when the button press or
	//! release was consumed.
	//!
	//! The button event is always preceded by a motion event, reporting
	//! the pointer's x & y coordinates.
	//!
	//! The default implementation always returns false.

	virtual bool process_button_event(IN_THREAD_ONLY,
					  const button_event &be,
					  xcb_timestamp_t timestamp);

	//! Whether this element will use the X Input Method.

	//! The default implementation returns false. When the element
	//! returns true, text entered via XIM gets pasted().

	virtual bool uses_input_method();

	//! Current cursor position when this element uses_input_method().

	//! Reports the current cursor position to the XIM server. Should
	//! only be called when this element has keyboard focus.
	void report_current_cursor_position(IN_THREAD_ONLY, rectangle pos);

	//! process_button_event() calls grab() to grab the pointer.

	virtual void grab(IN_THREAD_ONLY)=0;

	//! Invoke the pointer_focus() virtual function.
	void report_pointer_focus(IN_THREAD_ONLY,
				  focus_change event);

	//! The result of all this is a bunch of calls to pointer_focus().

	//! The default implementation invokes the pointer focus callback.

	virtual void pointer_focus(IN_THREAD_ONLY);

	//! Install a new keyboard focus callback
	void on_pointer_focus(const std::function<focus_callback_t> &callback);

	//! Install a new keyboard focus callback
	void on_pointer_focus(IN_THREAD_ONLY,
			      const std::function<focus_callback_t> &callback);
 private:
	//! Invoke the callback
	void invoke_pointer_focus_callback(IN_THREAD_ONLY);
 public:

	//! The pointer has moved.

	//! Log it, and if it's a real pointer movement trigger
	//! hover processing, if appropriate.
	virtual void report_motion_event(IN_THREAD_ONLY, const motion_event &);

	//! If this element has a hover action, schedule it.

	//! If hover_action_delay() returns a non-0 delay, make sure that
	//! something will look at it, after the prescribed interval
	//! expires.
	//!
	//! Overriden in child_elementObj, which invokes this and also
	//! invokes its container schedule_hover_action().
	virtual void schedule_hover_action(IN_THREAD_ONLY);

	//! Left focus, etc...

	//! Don't schedule the hover action any more.
	//!
	//! Overriden in child_elementObj, which invokes this and also
	//! invokes its container unschedule_hover_action().
	virtual void unschedule_hover_action(IN_THREAD_ONLY);

	//! If this element takes action on a hover delay, return the delay.

	//! A display element that implements a hover-triggered action
	//! implements hover_action_delay() together with hover_action()
	//! and hover_cancel().
	//!
	//! Returns 0 if this display element does not have a hover-triggered
	//! action, at the moment, otherwise the delay before the action
	//! takes place.
	//!
	//! The default implementation returns the tooltip delay
	//! configuration setting if this element has a tooltip factory
	//! installed, else returns 0.

	virtual std::chrono::milliseconds hover_action_delay(IN_THREAD_ONLY);

	//! The pointer has hovered over this element, so do something.

	//! A display element that implements a hover-triggered action
	//! implements hover_action_delay() together with hover_action()
	//! and hover_cancel().
	//!
	//! Only gets invoked when hover_action_delay() returned a non-0
	//! interval, which is now elapsed.
	//!
	//! The default implementation shows the tooltip.
	virtual void hover_action(IN_THREAD_ONLY);

	//! Cancel the hover-triggered action.

	//! A display element that implements a hover-triggered action
	//! implements hover_action_delay() together with hover_action()
	//! and hover_cancel().
	//!
	//! Gets invoked by unschedule_hover_action().
	//!
	//! The default implementation removes the tooltip, if one is
	//! currently shown.
	//! interval, which is now elapsed.
	virtual void hover_cancel(IN_THREAD_ONLY);

 private:
	//! Schedule a timer to check if the tooltip needs to be shown.
	void schedule_hover_timer(IN_THREAD_ONLY,
				    tick_clock_t::time_point now);

	//! Check if the tooltip needs to be shown.
	void check_hover_timer(IN_THREAD_ONLY);

	//! The most recent keyboard_focus() reported
	focus_change most_recent_keyboard_focus_change_thread_only=focus_change::lost;

	//! The most recent pointer_focus() reported
	focus_change most_recent_pointer_focus_change_thread_only=focus_change::lost;

 public:

	THREAD_DATA_ONLY(most_recent_keyboard_focus_change);
	THREAD_DATA_ONLY(most_recent_pointer_focus_change);

	//! The window lost or gained input focus

	//! The default implementation does nothing.

	virtual void window_focus_change(IN_THREAD_ONLY, bool flag);

	//! Whether this display element currently has keyboard input focus.

	//! Returns true if most_recent_keyboard_focus_change was anything
	//! other than focus_change::lost or focus_change::child_lost

	bool current_keyboard_focus(IN_THREAD_ONLY);

	//! Whether this display element currently has pointer input focus.

	//! Returns true if most_recent_pointer_focus_change was anything
	//! other than focus_change::lost or focus_change::child_lost

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

	//! Ensure visibility of this entire element.

	//! Constructs a rectangle for this entire display element's current
	//! size and invokes ensure_visibility().
	void ensure_entire_visibility(IN_THREAD_ONLY);

	//! Process pasted text.

	//! Returns true if this element processed the pasted text.
	//! The default elementation returns false.

	virtual bool pasted(IN_THREAD_ONLY,
			    const std::u32string_view &str);

	//! Creating a focusable element in this container.

	//! The default implementation does nothing. child_elementObj also
	//! forwards it to its parent container.

	virtual void creating_focusable_element();

	//! Grab the pointer for this element.

	//! Returns a mcguffin if the pointer is not already grabbed by another
	//! element, and the grab succeeds.

	grabbed_pointerptr grab_pointer(IN_THREAD_ONLY);

	//! The the mouse pointer pursor.
	void set_cursor_pointer(IN_THREAD_ONLY, const cursor_pointer &);

	//! Remove the mouse pointer cursor,
	void remove_cursor_pointer(IN_THREAD_ONLY);

	//! Retrieve the cursor pointer for this element.

	//! Overridden in child_elementObj. If this element does not have a
	//! set cursor pointer, call get_container_cursor_pointer().

	virtual cursor_pointerptr get_cursor_pointer(IN_THREAD_ONLY);
};

LIBCXXW_NAMESPACE_END

#endif
