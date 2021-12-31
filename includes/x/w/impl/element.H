/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_element_h
#define x_w_impl_element_h

#include <x/w/elementfwd.H>
#include <x/w/elementobj.H>
#include <x/w/rectangle.H>
#include <x/w/screenfwd.H>
#include <x/w/generic_windowobj.H>
#include <x/w/element_state.H>
#include <x/w/pictformatfwd.H>
#include <x/w/scratch_bufferfwd.H>
#include <x/w/input_mask.H>
#include <x/w/button_eventfwd.H>
#include <x/w/motion_eventfwd.H>
#include <x/w/labelfwd.H>
#include <x/w/busyfwd.H>
#include <x/w/focus.H>
#include <x/w/rgbfwd.H>
#include <x/w/focusableobj.H>
#include <x/w/focusable_labelfwd.H>
#include <x/w/tooltip_appearancefwd.H>
#include <x/w/font_arg.H>
#include <x/w/text_paramfwd.H>
#include <x/w/impl/redraw_priority.H>
#include <x/w/impl/clock.H>
#include <x/w/impl/connection_threadfwd.H>
#include <x/w/impl/updated_position_infofwd.H>
#include <x/w/defaultthemefwd.H>
#include <x/w/fontcollectionfwd.H>
#include <x/w/richtext/richtextstringfwd.H>
#include <x/w/impl/inherited_visibility_infofwd.H>
#include <x/w/impl/element_draw.H>
#include <x/w/impl/draw_info.H>
#include <x/w/impl/background_colorfwd.H>
#include <x/w/impl/current_border_implfwd.H>
#include <x/w/impl/grabbed_pointerfwd.H>
#include <x/w/impl/cursor_pointerfwd.H>
#include <x/w/impl/metrics_horizvertobj.H>
#include <x/w/impl/focus/label_forfwd.H>
#include <x/w/impl/focus/focusablefwd.H>
#include <x/w/impl/fonts/current_fontcollectionfwd.H>
#include <x/w/impl/popup/popupfwd.H>
#include <x/w/impl/dnd.H>
#include <x/w/impl/richtext/richtextmetafwd.H>
#include <x/w/impl/selection/current_selection_handlerfwd.H>
#include <x/w/key_eventfwd.H>
#include <x/w/fontfwd.H>
#include <x/w/tooltipfwd.H>
#include <x/obj.H>
#include <x/logger.H>
#include <x/functional.H>
#include <x/functionalrefptr.H>
#include <string_view>
#include <queue>
#include <variant>
#include <xcb/xproto.h>

#include <chrono>

LIBCXXW_NAMESPACE_START

//! \class x::w::generic_windowObj::handlerObj
//!
//! TODO: documentation

//! \class x::w::peepholeObj
//!
//! TODO: documentation

//! \class x::w::peepholeObj::layoutmanager_implObj
//!
//! TODO: documentation

//! \class background_color_element_recalculated
//!
//! TODO: documentation

//! \class border
//!
//! TODO: documentation

//! \class focusframelayoutimplObj
//!
//! TODO: documentation

//! \class x::w::fontconfig::sortedlist
//!
//! TODO: documentation

//! \class x::w::richtextfragmentObj
//!
//! TODO: documentation

class clip_region_set;

//! create_richtextstring: no hotspots are allowed

struct LIBCXX_HIDDEN hotspots_none {};

//! create_richtextstring: hotspots are allowed

//! A trailing NULL byte is inserted, in the
//! event that the string ends in a hotspot.
struct LIBCXX_HIDDEN hotspots_create {
	const label_hotspots_t &hotspots;
};

//! create_richtextstring: updating an existing hotspot

struct LIBCXX_HIDDEN hotspots_update {};

//! Whether create_richtextstring() should create focusable label hotspots.

//! Used in elementObj::implObj to determine whether hotspots are allowed
//! in the rich text string.

typedef std::variant<hotspots_none,
		     hotspots_create,
		     hotspots_update> hotspot_processing;

//! Base class for widget implementation.

//! Stuff that all widgets have.

class elementObj::implObj : public element_drawObj,

// horizvertObj is considered to be ONLY IN_THREAD
//
// **** use get_horizvert() to access it. ****
					  private metrics::horizvertObj {

 public:
	LOG_CLASS_SCOPE;
 private:

	struct LIBCXX_HIDDEN data_thread_only_t;

	struct LIBCXX_HIDDEN movable_rectangle_t;

	//! Information about the movable/scrollable rectangle in this widget.

	//! The value returned by has_scrollable_window_pixmap_rectangle().
	struct movable_rectangle_t {

		//! This widget's scrollable or movable rectangle
		rectangle r;

		//! Whether the rectangle can actually be moved.

		//! Otherwise this rectangle still exist in window_pixmap,
		//! but cannot be moved for some reason.

		bool is_movable;

		//! Constructor
		movable_rectangle_t(const rectangle &r) : r{r},
							  is_movable(false)
		{
		}
	};

	//! Most of the stuff is for the connection thread's consumption, only.

	struct data_thread_only_t {

		//! Current position

		//! The X & Y coordinates are relative to parent's

		rectangle current_position;

		//! Previous position

		//! The grid layout manager schedules itself for position update
		//! processing if its grid metrics change in order to reposition
		//! its child elements, rather than itself. But if in the
		//! meantime its position gets recalculated this will also
		//! reposition its child elements.
		//!
		//! This is the element's previous position. If its
		//! current_position really changes, we call process_updated
		//! position() and update it after process_updated_position()
		//! returns. If it's the same we call process_same_position()
		//! instead.

		rectangle previous_position;

		//! Whether this widget's contents can be moved directly

		//! Helps in avoiding redrawing of the widget in its new
		//! position, from scratch. Computed by
		//! update_current_position() using
		//! has_scrollable_window_pixmap_rectangle().
		//!
		//! This is set with is_movable=true in order to
		//! represent that this rectangle in the window_pixmap
		//! contains valid contents of this widget that can be copied
		//! to this widget's new position instead of redrawing the
		//! widget from scratch.
		//!
		//! This is cleared when we detect that this is no longer
		//! the case:
		//!
		//! In background_color_changed() and theme_updated, since
		//! the widget's contents are no longer true.
		//!
		//! Finally, can_be_moved() gets invoked during position
		//! processing. can_be_moved() factors in the current
		//! value of movable_rectangle, if there is one, then
		//! clears it.
		//!
		//! When this widget has a valid rectangle in the window_pixmap
		//! but it cannot be scrolled for some other reason, this
		//! is set but with is_movable=false.

		std::optional<movable_rectangle_t> movable_rectangle;

		//! This element's attached popup.
		popupptr attached_popup;

		//! Cached draw_info

		//! get_draw_info() retrieves the cached draw_info, or
		//! calls get_draw_info_from_scratch().
		//!
		//! invalidate_cached_draw_info() gets called whenever
		//! the cached draw info may no longer be valid for this
		//! widget.

		std::optional<draw_info> cached_draw_info;

		//! Which parts of the widget need redrawing.

		//! The default state is an empty rectarea. When an
		//! schedule_full_redraw() is called, this is removed.

		std::optional<rectarea> areas_to_redraw=rectarea{};

		////////////////////////////////////////////////////////////
		//
		// Defaulted values.

		//! Requested visibility.

		//! show()/hide() sets this, then adds this widget to the
		//! visibility_updated set.

		bool requested_visibility=false;

		//! Actual visibility.

		//! When the connection thread is done with its
		//! work, it goes through all elements in visibility_updated
		//! and if requested_visibility != actual_visibility,
		//! update_visibility gets called.

		bool actual_visibility=false;

		//! Inherited visibility

		//! An widget is actually visible only if the element,
		//! and all of its parent elements are visible.

		bool logical_inherited_visibility=false;

		//! Inherited visibility

		//! reported_inherited_visibility is always the same as
		//! logical_inherited_visibility except for top level display
		//! elements, because top_level_always_visible(). Top level
		//! elements are always considered to be visible, and their
		//! logical_inherited_visibility is always true, only their
		//! reported_inherited_visibility gets updated in response
		//! to show()ing or hide()ing them.
		//!
		//! When determining whether the widget is visible
		//! the library code looks at logical_inherited_visibility
		//! except when:
		//!
		//! - need to determine whether the top level window is mapped
		//! or not (this is reflected by reported_inherited_visibility).
		//!
		//! - when reporting the visibility status of a widget,
		//! so that the visibility report reflects the top level
		//! widget's visibility.
		//!
		//! - in removed_from_container, to determine whether or not
		//! it needs to synthesize a fake hide event, as the
		//! widget is getting removed.

		bool reported_inherited_visibility=false;

		//! Whether initialize() has been called.

		bool initialized=false;

		//! Whether the widget has been removed from its container.

		//! This is set to true in removed_from_container(), which
		//! gets scheduled to be called IN_THREAD from the public
		//! object's destructor.
		//!
		//! It is not set anywhere else.

		bool removed=false;

		//! Whether this widget is "enabled".

		//! If this is an widget that receives pointer and keyboard
		//! focus, it can be selectively disabled. If this widget
		//! is disabled, we draw a shade over its contents.
		//!
		//! The enabled() method checks not only this element's
		//! enabled flag, but also all of its parent containers.
		//! If any of them are disabled this widget is considered to
		//! be disabled. The enabled flag only specifies whether
		//! this widget is explicitly set as enabled or not.

		bool enabled=true;

		//! State update callback
		functionptr<element_state_callback_t> element_state_callback;

		//! Metrics update callback
		functionptr<metrics_update_callback_t> metrics_update_callback;

		//! Pointer focus callback

		functionptr<focus_callback_t> on_pointer_callback;

		//! Keyboard focus callback
		functionptr<focus_callback_t> on_keyboard_callback;

		//! Callback for reporting key events.

		functionptr<key_event_callback_t> on_key_event_callback;

		//! Callback for reporting button events.

		functionptr<button_event_callback_t> on_button_event_callback;

		//! Callback for reporting motion events.

		functionptr<motion_event_callback_t> on_motion_event_callback;

		//! Whether this widget is a label for some focusable.

		label_forptr label_for;

		//! Most recently reported motion

		coord_t last_motion_x=0;

		//! Most recently reported motion

		coord_t last_motion_y=0;

		//! This element's current cursor pointer, if there is one.
		cursor_pointerptr pointer;

		//! This element's tooltip factory
		functionptr<void (THREAD_CALLBACK,
				  const tooltip_factory &)> tooltip_factory;

		//! There's a popup attached to this widget.

		//! This is used by combo-box code to attach the implementation
		//! object for the combo-box's popup, so it gets hidden if
		//! the combo-box widget itself gets hidden.

		element_implptr attached_popup_impl;

		//! Context popup callback.
		functionptr<void(THREAD_CALLBACK, const callback_trigger_t &,
				 const busy &)
			      > contextpopup_callback;

		//! Scheduled tooltip creation
		tick_clock_t::time_point hover_scheduled_creation;

		//! Timer for the scheduled tooltip creation
		ptr<obj> hover_scheduled_mcguffin;
	};

	//! For the connection thread's use, only.

	data_thread_only_t data_thread_only;

 protected:
	//! Invoked from always_visible_elementObj's constructor

	inline void set_always_visible()
	{
		data_thread_only.requested_visibility=true;
	}

	//! Invoked from generic_windowObj::HandlerObj's constrcutor.

	inline void top_level_always_visible()
	{
		data_thread_only.logical_inherited_visibility=true;
		data_thread_only.actual_visibility=true;
	}
 public:

	//! Use parent widget's background color.

	//! Schedules the virtual method to be invoked by the connection thread.

	void remove_background_color();

	//! Use parent widget's background color.

	virtual void remove_background_color(ONLY IN_THREAD)=0;


	//! Use a standard background color for this widget.

	void set_background_color(const color_arg &theme_color);

	//! Use a standard background color for this widget.

	void set_background_color(ONLY IN_THREAD,
				  const color_arg &theme_color);
	//! Set the background color.

	//! Schedules the virtual method to be invoked by the connection thread.

	void set_background_color(const background_color &);

	//! Install the background color

	virtual void set_background_color(ONLY IN_THREAD,
					  const background_color &)=0;

	//! Whether this widget uses its own custom background color

	//! The default implementation always returns true.
	//! child_element overrides this, and returns the correct values.

	virtual bool has_own_background_color(ONLY IN_THREAD)=0;

	//! Retrieve the effective background color

	//! If has_own_backgorund_color(), returns it, else returns the
	//! parent element's background color.
	virtual background_color current_background_color(ONLY IN_THREAD)=0;

	//! This element's background color has changed.

	//! Invoke schedule_full_redraw() if this widget is visible.
	//! But wait, there's more!. If this is a container, and the
	//! container has any visible child elements that use the parent's
	//! background color, they need to be redrawn too!

	virtual void background_color_changed(ONLY IN_THREAD);

	//! Return the element's screen
	screen get_screen();

	//! Return the element's screen
	const_screen get_screen() const;

	//! Nesting level.

	//! Top level window elements have a nesting level of 0.
	//! All others have one more than their parent widget.
	const size_t nesting_level;

	//! Available scratch buffer, for drawing in this widget.

	const scratch_buffer element_scratch_buffer;

	//! Constructor
	implObj(size_t nesting_level,
		generic_windowObj::handlerObj &generic_window_handler,
		const rectangle &initial_position,
		const popupptr &attached_popup,
		const metrics::horizvert_axi &initial_metrics,
		const screen &my_screen,
		const const_pictformat &my_pictformat,
		const std::string &scratch_buffer_id);

	//! Destructor
	~implObj();

	THREAD_DATA_ONLY(data);
	THREAD_DATA_SUPERCLASS_ONLY(horizvertObj, get_horizvert);

 protected:

	//! Drawing codepaths need to bail out if DO_NOT_DRAW().

	inline bool DO_NOT_DRAW(ONLY IN_THREAD) const
	{
		return (!data(IN_THREAD).logical_inherited_visibility);
	}
 public:
	//! Install minimum widget size overrides.
	void set_minimum_override(dim_t horiz_override, dim_t vert_override);

	//! Install minimum widget size overrides.
	virtual void set_minimum_override(ONLY IN_THREAD,
					  dim_t horiz_override,
					  dim_t vert_override);

	//! The container has removed this widget.

	//! Invoked from elementObj's destructor.
	//! Runs removed_from_container() IN_THREAD.

	void removed_from_container() LIBCXX_HIDDEN;

	//! The container has removed this widget.

	//! Executed in the connection thread.
	//! Recursively invokes removed_from_container if this widget itself
	//! is a container.
	//!
	//! If data.removed is not set, sets it and invoked removed().

	virtual void removed_from_container(ONLY IN_THREAD);

	//! This widget has been removed from its container just now.

	//! The default implementation does nothing.
	virtual void removed(ONLY IN_THREAD);

	//! Install a state update callback.
	void on_state_update(const functionref<element_state_callback_t> &cb);

	//! Install a state update callback.
	void on_state_update(ONLY IN_THREAD,
			     const functionref<element_state_callback_t> &cb);
	//! Install a metrics update callback.

	void on_metrics_update(const functionref<metrics_update_callback_t>
			       &cb);

	//! Somebody wishes to inform us of our new current_position

	//! Invoked by a layout manager, to update one of it's elements'
	//! position.
	//!
	//! If the given rectangle is different than the existing current
	//! position, update it, and call notify_updated_position()
	//! and current_position_updated().

	void update_current_position(ONLY IN_THREAD,
				     const rectangle &r);

	//! Whether this widget can be scrolled or repositioned directly.

	//! If so, returns this widget viewport, in the window, that
	//! can be scrolled or moved, with is_movable.
	//!
	//! This is used when scrolling a widget in a peephole, for example.
	//! This is a shortcut to determine whether we can
	//! scroll_window_pixmap() directly, for this.
	//!
	//! One of the criteria is that the widget's background color must
	//! be a solid color (a scrollable background), and cannot be
	//! a gradient or a picture.
	//!
	//! However this is also used when determining if it's possible to
	//! avoid redrawing the widget from scratch when moving it inside
	//! the window. In which case, if the non-scrollable background is
	//! the widget's own background it can still be scrolled, but if
	//! it is drawn on top of its parent widget's non-scrollable background
	//! it cannot be moved.

	std::optional<movable_rectangle_t>
	has_scrollable_window_pixmap_rectangle
	(ONLY IN_THREAD,
	 bool my_nonscrollable_background_is_ok);

	//! Determine whether this widget's contents can be moved

	//! This is called during position update processing to determine
	//! if the widget has already been drawn, so instead of redrawing
	//! the widget from scratch in its new position it's possible
	//! to just copy the widget in the window_pixmap to its new position.
	//!
	//! Compares previous_position and updated_position. If the width
	//! and the height are the same, then we do further sanity checks,
	//! such as whether there is a movable_rectangle (we already determined
	//! that), and several other things.
	//!
	//! If everything looks good, my_move_iterator and some additional
	//! metadata get opcied into the move_container, indicating what, when
	//! where, and how this widget can_be_moved().
	//!
	//! In all cases, any movable_rectangle gets removed, this is the
	//! last call for the move express!

	void can_be_moved(ONLY IN_THREAD,
			  all_updated_position_widgets_t_iterator
			  my_move_iterator,
			  updated_position_container_t &move_container);

	//! The parent container is scrolling us.

	//! This is semantically equivalent to update_current_position(),
	//! except that current_position_updated() does not get invoked.
	//! Updates this element's position, and invalidates the cached
	//! draw info. The parent container scrolling us will be responsible
	//! for redrawing everything, we're just doing the bookkeeping here.

	void scroll_by_parent_container(ONLY IN_THREAD, coord_t x, coord_t y);

	//! The top level window's position changed, but not its size

	//! So nothing has to be recalculated, however popups need to know
	//! about this.
	//!
	//! Propagate absolute_location_updated() to all elements.
	//!
	//! redraw_after_absolute_location_updated() must be invoked when
	//! we return.

	virtual void absolute_location_updated(ONLY IN_THREAD);

	//! What was the reason for updating the absolute location only.

	enum class absolute_location_update_reason {
		external, //!< Our top level window has moved
		scrolled, //!< The widget was scrolled by its parent
		internal  //!< The widget moved, but without changing its size
	};

	//! What was the reason for collecting exposure_event_recursive

	enum class exposure_type {
	       actual_exposure, //!< The X server told us so.
	       scrolled,       //!< We were scrolled
	};

	//! Ok, now invalidate, and possibly redraw ourselves.

	//! If the absolute_location_update_reason is internal, we need to
	//! redraw ourselves. If it's external, somebody took care of that.

	void redraw_after_absolute_location_updated
	(ONLY IN_THREAD,
	 absolute_location_update_reason reason);

	//! This element's current_position has been updated.

	//! Invoke schedule_update_position_processing(), then recursively
	//! absolute_location_updated() (and
	//! redraw_after_absolute_location_updated) for all child elements.
	//! The net effective is that this element, and its child elements,
	//! are scheduled to be redrawn.
	//!
	//! generic_window_handler overrides this. A redraw is not required
	//! when the top level window's position changed. If its size also
	//! changed, and the top level window's layout manager has to reposition
	//! the child elements, that will itself trigger a redraw.

	virtual void current_position_updated(ONLY IN_THREAD);

	//! Indicate whether this widget is already updated.

	//! But process_updated_position() was not called yet.
	bool update_position_processing_scheduled(ONLY IN_THREAD);

	//! Add this widget to the element_position_updated() queue.

	//! Adds this widget to the queue that the connection thread uses
	//! to invoke process_updated_position().
	//!
	//! generic_window_handler invokes this directly, bypassing
	//! current_position_updated(). It is not necessary to recursively
	//! do this for all widgets when the top level window is
	//! resized or moved. It is only necessary to schedule the top level
	//! window to execute current_position_updated(), as the top level
	//! container will recalculate and reposition all elements, as needed.

	void schedule_update_position_processing(ONLY IN_THREAD) LIBCXX_HIDDEN;

	//! Invoked by the connection thread.

	//! The default implementation invokes schedule_full_redraw().
	//!
	//! Overridden by containers, which notify their layout manager,
	//! before invoking notify_updated_position().
	//!
	//! redraw_after_process_updated_position() must be invoked when
	//! we return.

	virtual void process_updated_position(ONLY IN_THREAD);

	//! Do we need to redraw ourselves after we process_updated_position?

	void redraw_after_process_updated_position(ONLY IN_THREAD,
						   updated_position_info &info);

	//! Invoked by the connection thread.

	//! The default implementation does nothing.
	//!
	//! OVerridden by containers, which notify their layout manager.

	virtual void process_same_position(ONLY IN_THREAD);

	//! Calls invoke_element_state_updates().

	void notify_updated_position(ONLY IN_THREAD) LIBCXX_HIDDEN;

	//! Create the current element_state for invoking callbacks.
	element_state create_element_state(ONLY IN_THREAD,
					   element_state::state_update_t)
		LIBCXX_HIDDEN;

	//! Invoke all existing state update callbacks, for some reason.

	virtual void invoke_element_state_updates(ONLY IN_THREAD,
						  element_state::state_update_t)
		;

	/////////////////////////////////////////////////////////////////////
	//

	//! show() or hide() this widget.

	void request_visibility(bool flag);

	//! Show or hide() this widget.

	void toggle_visibility();

	//! show() or hide() this widget.

	//! Executed in the connection thread.
	//!
	//! Invokes schedule_update_visibility().
	//!
	//! Overridden in generic_window_handler(). The first time a top
	//! level window is made visible, it gets resized to its preferred
	//! width, first.
	//!
	//! Overridden in child_element, the grid layout manager may choose
	//! to hide non-visible elements.
	virtual void request_visibility(ONLY IN_THREAD, bool flag);

	//! show() or hide() this widget.

	//! Executed in the connection thread.
	void toggle_visibility(ONLY IN_THREAD);

	//! Schedule update_visibility() to be invoke by the connection thread.

	//! Inserts this widget into the visibility_update list.

	void schedule_update_visibility(ONLY IN_THREAD) LIBCXX_HIDDEN;

	//! Recursive show() or hide().

	void request_visibility_recursive(bool flag);

	//! Recursive show() or hide().

	//! Executed in the connection thread.
	//! The default implementation invokes request_visibility().
	//! Overridden by the container implementation object.

	virtual void request_visibility_recursive(ONLY IN_THREAD, bool flag);

	//! Executed in the connection thread.

	//! If different visibility was updated, invoke update_visibility().

	virtual void update_visibility(ONLY IN_THREAD);

	//! Hold of on update_visibility while resize_pending

	//! Returns false by default. Overridden int
	//! generic_windowObj::handlerObj overrides and returns true.
	//! It maps the window, which is needed for resizing processing.

	virtual bool update_visibility_while_resize_pending();

	//! update_visibility() invokes this callback.

	//! This is overridden in child_elementObj, which also checks
	//! the parent's inherited visibility. If the parent is hidden,
	//! this widget is not visible, and flag is forced to off, before
	//! invoking this method.
	//!
	//! visibility_updated() compares flag with the
	//! current value of inherited_visibility flag. If it's different,
	//! inherited_visibility_updated() gets invoked.
	virtual void visibility_updated(ONLY IN_THREAD, bool flag);

	//! This element's inherited_visibility flag has changed.

	//! This default implementation calls do_inherited_visibility_updated().
	//! If this is a container, this gets overridden, and the container
	//! also updates the containers' elements inherited visibility,
	//! before invoking do_inherited_visibility_updated().

	virtual void inherited_visibility_updated(ONLY IN_THREAD,
						  inherited_visibility_info &)
		LIBCXX_PUBLIC;

	//! Finally update this element's inherited visibility status.

	//! The relationships with the parent widget and any child elements
	//! (if this is a container), have been squared away.
	//!
	//! Update the flag, invoke before/after showing/hiding callbacks.

	void do_inherited_visibility_updated(ONLY IN_THREAD,
					     inherited_visibility_info &info);

	//! Actually set the inherited visibility flag, and I do really mean it.

	//! do_inherited_visibility_updated() invokes set_inherited_visibility()
	//! after invoking the before_showing/hiding callbacks, and before
	//! invoking the after_showing/hiding callbacks.
	//!
	//! The default implementation sets data.inherited_visibility.
	//!
	//! Overridden in child_elementObj to also invoke its container's
	//! child_background_color_changed() callback, since when the
	//! child widget is not shown, it falls back to using the parent's
	//! background color, so officially its background color has changed.

	virtual void set_inherited_visibility(ONLY IN_THREAD,
					      inherited_visibility_info
					      &visibility_info)
		LIBCXX_PUBLIC;

	//! Ditto

	//! Implement set_inherited_visibility flag.
	virtual void set_inherited_visibility_flag(ONLY IN_THREAD,
						   bool, bool);

	//! Redraw this widget, after showing or hiding it.

	//! visibility_updated() invokes it after it invokes
	//! inherited_visibility_updated().
	//!
	//! The default implementation calls schedule_full_redraw(). Overridden
	//! by containers to draw their elements after they become visible.
	//! The containers draw their own area by overriding do_draw().
	//!
	//! Overridden by child_element, calling the container's
	//! child_visibility_updated().

	virtual void draw_after_visibility_updated(ONLY IN_THREAD,
						   bool flag);

	//! Schedule ourselves for finalized position processing

	//! Schedules process_finalized_position() to be invoked after
	//! all updated widger positions have been set.

	void schedule_finalized_position(ONLY IN_THREAD);

	//! Invoked by the connection thread.

	//! Invoked after there are no more position or visibility updates,
	//! and no resize is pending for the top level window.

	virtual void process_finalized_position(ONLY IN_THREAD);

	//! Return the cached_draw_info for this widget.

	//! Call get_draw_info_from_scratch() if it does not exist.

	virtual draw_info &get_draw_info(ONLY IN_THREAD)=0;

	//! Calculate draw_info for this widget.

	virtual draw_info &get_draw_info_from_scratch(ONLY IN_THREAD)=0;

	//! Why invalidate_cached_draw_info() was called.

	enum class draw_info_invalidation_reason {

		//! Something that factored into cached_draw_info for this
		//! widget might've changed.
		something_changed,

		//! Something definitely changed, remove any cached_draw_info.
		recursive_invalidation
	};

	//! The cached_draw_info is no longer valid.

	//! Something that factors into draw_info's contents has changed,
	//! so if it is cached make sure it is still valid.

	void invalidate_cached_draw_info(ONLY IN_THREAD,
					 draw_info_invalidation_reason);

	//! We don't need the full draw_info, just the absolute location.

	virtual rectangle get_absolute_location(ONLY IN_THREAD) const=0;

	//! We need get_absolute_location() for the screen!
	rectangle get_absolute_location_on_screen(ONLY IN_THREAD) const;

	//! Whether this widget can be under the mouse pointer.

	//! Returns true if this widget is eligible to be consider to be
	//! under the mouse pointer.

	bool can_be_under_pointer(ONLY IN_THREAD) const;

	//! Schedule the connection thread to invoke explicit_redraw().

	void schedule_full_redraw(ONLY IN_THREAD);

	//! Schedule the connection thread to invoke explicit_redraw().

	//! Add the area to areas_to_redraw(), unless it's been cleared for
	//! a full redraw.
	void schedule_redraw(ONLY IN_THREAD, const rectangle &area);

	//! schedule_full_redraw() this one, and all child elements.

	void schedule_redraw_recursively();

	//! schedule_full_redraw() this one, and all child elements.

	//! Each element is added to the scheduled list, and recursion stops
	//! if it's already there.
	void schedule_redraw_recursively(ONLY IN_THREAD,
					 std::unordered_set<element_impl
					 > &scheduled);

	//! This element's enabled status has changed.

	//! Calls schedule_redraw() and invokes itself recursively for all
	//! child elements (if any).
	//!
	//! Disabled elements are drawn in shaded style, so when an widget
	//! becomes enabled or disabled we schedule_redraw().
	//!
	//! Additionally, an widget marked as disabled automatically
	//! disables all child elements.

	virtual void enablability_changed(ONLY IN_THREAD);

	//! Whether this widget is scheduled for complete redrawing

	bool full_redraw_scheduled(ONLY IN_THREAD);

	//! Forcibly redraw everything in this widget

	//! Not only areas_to_redraw, but everything, including child widges.

	void explicit_redraw_recursively(ONLY IN_THREAD);

	//! Forcibly redraw this widget

	//! If this widget is not visible, calls clear_to_color().
	//! Otherwise calls draw() (which then has to call do_draw(), because
	//! the widget is visible).
	//!
	//! explicit_redraw() only redraws the areas_to_redraw which were
	//! requested to be redrawn.

	void explicit_redraw(ONLY IN_THREAD);

	//! This widget's redrawing priority
	virtual redraw_priority_t get_redraw_priority(ONLY IN_THREAD);

	//! Exposure event handling.

	//! We get areas to redraw for this widget. Invoke draw(),
	//! then recursively invoke ourselves for all visible children,
	//! after calculating each child's rectarea consisting of the
	//! given rectarea intersected with the child's viewport.

	void exposure_event_recursive(ONLY IN_THREAD,
				      const rectarea &areas,
				      exposure_type type)
		LIBCXX_HIDDEN;

	//! Draw the widget.

	//! If the widget is not visible, does nothing.
	//!
	//! Otherwise calls do_draw().
	void draw(ONLY IN_THREAD,
		  const draw_info &di,
		  const rectarea &areas) LIBCXX_HIDDEN;

	//! Widget has been drawn.

	//! Invoked by draw().

	void drawn(ONLY IN_THREAD) LIBCXX_HIDDEN;

	//! Draw the widget.

	//! The default implementation invokes clear_to_color().
	//!
	//! The rectangle cooridnates are specified relative to the viewport.

	virtual void do_draw(ONLY IN_THREAD,
			     const draw_info &di,
			     const rectarea &areas);


	//! Implementation of draw_using_scratch_buffer() with element_scratch_uffer.
	void do_draw_using_scratch_buffer(ONLY IN_THREAD,
					  const function
					  <scratch_buffer_draw_func_t> &cb,
					  const rectangle &rect,
					  const draw_info &di,
					  const draw_info &background_color_di,
					  const clip_region_set &clipped)
		override;

	//! Implementation of draw_using_scratch_buffer() with a custom scratch buffer.
	void do_draw_using_scratch_buffer(ONLY IN_THREAD,
					  const function
					  <scratch_buffer_draw_func_t> &cb,
					  const rectangle &rect,
					  const draw_info &di,
					  const draw_info &background_color_di,
					  const clip_region_set &clipped,
					  const scratch_buffer &buffer)
		override;

	//! A subclass drew all or a portion of this widget.

	//! Composite it into draw_info.window_picture. All drawing of this
	//! element's contents is ultimately funneled through draw_to_window
	//! picture.

	void draw_to_window_picture(//! Invoked in the connection thread
				    ONLY IN_THREAD,

				    //! The draw operation is now clipped
				    const clip_region_set &set,

				    //! This element's draw_info
				    const draw_info &di,

				    //! Where the subclass drew what it drew.

				    //! draw_to_window_picture() may make
				    //! further modifications.
				    const picture &contents,

				    //! Picture's pixmap.
				    const pixmap &contents_pixmap,

				    //! Which portion of this widget is drawn.

				    //! The subclass draws a portion of the
				    //! this widget specified by "rect"
				    //! in "contents".
				    const rectangle &rect)
		LIBCXX_HIDDEN;

	//! Whether draw_to_window_picture() should apply the disabled_mask

	//! Determines whether this widget should be shown as
	//! "disabled".
	//!
	//! Overridden in child_elementObj. Elements in a focusable label
	//! forward this to their container, in order to be drawn as disabled
	//! if their parent container is a label for a focusable.

	virtual bool draw_to_window_picture_as_disabled(ONLY IN_THREAD);

	//! Clear to background color

	//! Wrapper that acquires a clip_region_set, first.

	void clear_to_color(ONLY IN_THREAD,
			    const draw_info &di,
			    const rectarea &areas);

	//! Clear to background color

	//! The rectangles' coordinates are relative to the viewport.
	//! background_color_di is the draw_info of the widget whose background
	//! color is drawn here (default is the same as di).
	void clear_to_color(ONLY IN_THREAD,
			    const clip_region_set &,
			    const draw_info &di,
			    const draw_info &background_color_di,
			    const rectarea &areas);


	//! clear_to_color() has cleared this area to the background color.

	//! The default implementation of do_draw() calls clear_to_color().
	//! clear_to_color() clears each area to draw to the background color,
	//! and calls clear_to_color_area(), passing:
	//!
	//! - picture, pixmap, and a gc for a scratch buffer with the cleared
	//! background color.
	//!
	//! - draw_info
	//!
	//! - which part of the element this is for. The coordinates are
	//! absolute.
	//!
	//! The default implementation does nothing, hence the default behavior
	//! is background color clear.

	virtual void cleared_to_background_color(ONLY IN_THREAD,
						 const picture &,
						 const pixmap &,
						 const gc &,
						 const draw_info &,
						 const rectangle &);

	//! A new theme has been installed.

	//! This is invoked from the window handler's theme_updated_event(),
	//! Invoke schedule_redraw(), then recursively invoke
	//! theme_updated() (as long as we're visible, of course).

	virtual void theme_updated(ONLY IN_THREAD,
				   const const_defaulttheme &new_theme);

	//! Invoke initialize(), if necessary.

	//! Returns "true" if this widget was initialized (first time
	//! its container looked at it).

	inline bool initialize_if_needed(ONLY IN_THREAD)
	{
		auto &flag=data(IN_THREAD).initialized;

		if (flag)
			return false; // Already initialized

		flag=true;

		// update_visibility() bailed out if we were not initialized(),
		// so reschedule this.

		schedule_update_visibility(IN_THREAD);
		initialize_or_log_exception(IN_THREAD);

		return true;
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

	void initialize_or_log_exception(ONLY IN_THREAD) LIBCXX_HIDDEN;

 protected:
	//! Initialize this widget.

	//! After a widget gets added to a container, the layout
	//! manager should invoke initialize() from the connection thread.

	virtual void initialize(ONLY IN_THREAD);

	//! Whether this widget has been initialized

	inline bool initialized(ONLY IN_THREAD) const
	{
		return data(IN_THREAD).initialized;
	}
 public:
	//! Return a pointer to my parent widget.

	//! Implement in child_elementObj, and generic_windowObj::handlerObj
	//! which returns NULL.
	//!
	//! This is used for process_element_position_updated when it
	//! stores a reference to this widget. Because child widgets own
	//! references to their parents, this native pointer will remain
	//! valid.
	virtual implObj *get_parent_element_impl() const=0;

	//! Invoke a lambda for each child widget.

	template<typename functor_type>
		void for_each_child(ONLY IN_THREAD,
				    functor_type &&functor)
	{
		do_for_each_child(IN_THREAD,
				  make_function<void (const element &e)>
				  (std::forward<functor_type>(functor)));
	}

	//! Iterate over all child elements.

	//! The default implementation does nothing. This is overridden in
	//! containerObj.

	virtual void do_for_each_child(ONLY IN_THREAD,
				       const function<void
				       (const element &e)> &);

	//! Return the estimated number of children.

	//! The default implementation returns 0. This is overridden in
	//! containerObj.

	virtual size_t num_children(ONLY IN_THREAD);

	////////////////////////////////////////////////////////////////////
	//
	// Fonts, colors, text.


	//! Theme font for labels in this container.

	//! Overridden in child_elementObj and forwarded to the container.
	//!
	//! Overridden in generic_windowObj::handlerObj.

	virtual font_arg label_theme_font() const=0;

	//! Foreground color for labels in this container.

	//! Overridden in child_elementObj and forwarded to the container.
	//!
	//! Overridden in generic_windowObj::handlerObj.

	virtual color_arg label_theme_color() const=0;

	//! Convert a font_arg into a current_fontcollection

	current_fontcollection create_current_fontcollection(const font_arg &);

	//! Convert a font specification into a fontcollection
	fontcollection create_fontcollection(const font &);

	//! Convert a font specification into a fontcollection
	fontcollection create_fontcollection(const font &,
					     const defaulttheme &);

	//! Shortcut for invoking the same screen method

	background_color create_background_color(const color_arg &color_name);

	//! Create a border for this element.

	current_border_impl create_border(const border_arg &arg);

	//! Helper for creating a new scratch buffer.

	scratch_buffer create_scratch_buffer(const std::string &id);

	//! Helper for creating a new alpha channel scratch buffer.

	scratch_buffer create_alpha_scratch_buffer(const std::string &id,
						   depth_t depth=8);

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
					     hotspot_processing allow_links=
					     hotspots_none{});


	//////////////////////////////////////////////////////////////////
	//
	// Input focus handling

	//! A new focusable widget was initialized.

	//! A new focusable element always gets created as the last widget
	//! in the tabbing order.
	//!
	//! Overriden by child_elementObj, forwarding this call to its
	//! parent widget.
	//! Overriden in peephole_toplevel, to enforce that the top level
	//! peephole's scrollbars are always last in the tabbing order,
	//! repositioning this one's.
	//!
	//! Overriden in generic_windowObj::handlerObj, which ends up
	//! handling either top level windows without peepholes (tooltips),
	//! or the scrollbars themselves.
	virtual void focusable_initialized(ONLY IN_THREAD,
					   focusableObj::implObj &fimpl)=0;

	//! This element should be the first focusable widget.
	//! child_elementObj forwards it to its parent container; and this
	//! is implemented in generic_windowObj::handlerObj.

	virtual void get_focus_first(ONLY IN_THREAD, const focusable &)=0;

	//! Whether this widget is enabled for focus processing.

	//! Overridden in child_elementObj, to forward to the parent widget.
	//! Turning off focus in any parent widget automatically turns off
	//! focus processing for all of their child elements, in this manner.

	virtual bool enabled(ONLY IN_THREAD, enabled_for what);

	//! Used in input focus processing, accessed by the connection thread only.

	bool original_focus;

	//! Used in input focus processing, accessed by the connection thread only.

	bool new_focus;

	//! The focus reporter parameter to request_focus() and leave_focus().

	typedef void (elementObj::implObj::*focus_reporter_t)
		(ONLY IN_THREAD, focus_change, const callback_trigger_t &);

	//! Input focus is being requested for this widget.

	//! Generates calls to {type}_focus() for all affected widgets.
	//!
	//! focus_reporter parameter is either &report_keyboard_focus or
	//! &report_pointer_focus.
	//!
	//! \see focus_change

	void request_focus(ONLY IN_THREAD,
			   const ptr<elementObj::implObj> &current_focus,
			   focus_reporter_t focus_reporter,
			   const callback_trigger_t &trigger) LIBCXX_HIDDEN;

	//! Input focus is being lost completely.

	//! focus_reporter parameter is either &report_keyboard_focus or
	//! &report_pointer_focus.
	//!
	//! Generates calls to {type}_focus() for all affected widgets.
	//! \see focus_change
	//!
	//! In testfocusable.C this is equivalent to the (!t.to) case in
	//! testfocusable().

	void lose_focus(ONLY IN_THREAD,
			focus_reporter_t focus_reporter,
			const callback_trigger_t &trigger)
	{
		requested_focus_from(IN_THREAD);
		leaving_focus(IN_THREAD, ptr<elementObj::implObj>(),
			      focus_reporter, trigger);
		focus_movement_complete(IN_THREAD, false, focus_reporter,
					trigger);
	}

	//! Internal focus processing, see element_focusable.C

	virtual void requested_focus_to(ONLY IN_THREAD,
					const ptr<elementObj::implObj> &current_focus);

	//! Internal focus processing, see element_focusable.C
	virtual void requested_focus_from(ONLY IN_THREAD);

	//! Internal focus processing, see element_focusable.C

	void leaving_focus(ONLY IN_THREAD,
			   const ptr<elementObj::implObj> &leaving_for,
			   focus_reporter_t focus_reporter,
			   const callback_trigger_t &trigger)
		LIBCXX_HIDDEN;

	//! Internal focus processing, see element_focusable.C
	virtual void do_leaving_focus(ONLY IN_THREAD,
				      focus_change &event,
				      const ref<implObj> &focus_from,
				      const ptr<elementObj::implObj>
				      &leaving_for,
				      focus_reporter_t reporter,
				      const callback_trigger_t &trigger);

	//! Internal focus processing, see element_focusable.C

	void entering_focus(ONLY IN_THREAD,
			    const ptr<elementObj::implObj> &focus_from,
			    focus_reporter_t focus_reporter,
			    const callback_trigger_t &trigger)
		LIBCXX_HIDDEN;

	//! Internal focus processing, see element_focusable.C

	virtual void do_entering_focus(ONLY IN_THREAD,
				       focus_change event,
				       const ref<implObj> &focus_to,
				       const ptr<elementObj::implObj> &focus_from,
				       focus_reporter_t focus_reporter,
				       const callback_trigger_t &trigger);

	//! Internal focus processing, see element_focusable.C
	virtual void focus_movement_complete(ONLY IN_THREAD,
					     bool stop_at_original_focus,
					     focus_reporter_t focus_reporter,
					     const callback_trigger_t &trigger);

	//! Invoke the keyboard_focus() virtual function.

	void report_keyboard_focus(ONLY IN_THREAD,
				   focus_change event,
				   const callback_trigger_t &trigger)
		LIBCXX_HIDDEN;

	//! The result of all this is a bunch of calls to keyboard_focus().

	//! The default implementation invokes the keyboard focus callback.

	virtual void keyboard_focus(ONLY IN_THREAD,
				    const callback_trigger_t &trigger);

	//! Install a new keyboard focus callback
	void on_keyboard_focus(const functionref<focus_callback_t> &callback);

	//! Install a new keyboard focus callback
	void on_keyboard_focus(ONLY IN_THREAD,
			       const functionref<focus_callback_t> &callback);

	//! Install a new key event callback

	void on_key_event(const functionref<key_event_callback_t> &);

	//! Install a new key event callback

	void on_key_event(ONLY IN_THREAD,
			  const functionref<key_event_callback_t> &);


 private:
	//! Invoke the callback
	void invoke_keyboard_focus_callback(ONLY IN_THREAD,
					    const callback_trigger_t &)
		LIBCXX_HIDDEN;
 public:
	//! Here come the keypresses.

	//! Returns true or false, indicating whether the key press or
	//! release was consumed.
	//!
	//! The default implementation invokes the installed callback.

	virtual bool process_key_event(ONLY IN_THREAD, const key_event &ke);

	//! And here go the keypress

	//! A popup is shamelessly stealing my events. This is invoked
	//! every time a key release event gets stolen by a popup.
	//!
	//! The default implementation does nothing. Overridden by hotspots,
	//! to set their temperature to cold, when:
	//!
	//! - the hotspot becomes hot
	//!
	//! - the hotspot activates
	//!
	//! - the activation opens a popup which grabs the key events.
	//!
	//! - upon a key release event, this hook is used to bring the
	//! hotspot temperature to cold, again.

	virtual void grabbed_key_event(ONLY IN_THREAD);

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

	virtual bool process_button_event(ONLY IN_THREAD,
					  const button_event &be,
					  xcb_timestamp_t timestamp);

	//! If enabled() call process_button_event()

	virtual bool process_button_event_if_enabled(ONLY IN_THREAD,
						     const button_event &be,
						     xcb_timestamp_t timestamp);

	//! Invoke the context popup calback, if one is defined.

	//! If contextpopup_callback is set invoke it and return true.
	//! Returns false if contextpopup_callback is not set.

	bool invoke_contextpopup_callback(ONLY IN_THREAD,
					  const callback_trigger_t
					  &trigger);

	//! Whether this widget will use the X Input Method.

	//! The default implementation returns false. When the widget
	//! returns true, text entered via XIM gets pasted().

	virtual bool uses_input_method();

	//! Current cursor position when this widget uses_input_method().

	//! Reports the current cursor position to the XIM server. Should
	//! only be called when this widget has keyboard focus.
	void report_current_cursor_position(ONLY IN_THREAD, rectangle pos);

	//! process_button_event() calls grab() to grab the pointer.

	virtual void grab(ONLY IN_THREAD,
			  const callback_trigger_t &trigger) LIBCXX_PUBLIC=0;

	//! Invoke the pointer_focus() virtual function.
	void report_pointer_focus(ONLY IN_THREAD,
				  focus_change event,
				  const callback_trigger_t &trigger)
		LIBCXX_HIDDEN;

	//! The result of all this is a bunch of calls to pointer_focus().

	//! The default implementation invokes the pointer focus callback.

	virtual void pointer_focus(ONLY IN_THREAD,
				   const callback_trigger_t &trigger);

	//! Install a new keyboard focus callback
	void on_pointer_focus(const functionref<focus_callback_t> &callback);

	//! Install a new keyboard focus callback
	void on_pointer_focus(ONLY IN_THREAD,
			      const functionref<focus_callback_t> &callback);
 private:
	//! Invoke the callback
	void invoke_pointer_focus_callback(ONLY IN_THREAD,
					   const callback_trigger_t &)
		LIBCXX_HIDDEN;
 public:

	//! The pointer has moved.

	//! Log it, and if it's a real pointer movement trigger
	//! hover processing, if appropriate.
	virtual void report_motion_event(ONLY IN_THREAD, const motion_event &);

	//! If this widget has a hover action, schedule it.

	//! If hover_action_delay() returns a non-0 delay, make sure that
	//! something will look at it, after the prescribed interval
	//! expires.
	//!
	//! Overriden in child_elementObj, which invokes this and also
	//! invokes its container schedule_hover_action().
	virtual void schedule_hover_action(ONLY IN_THREAD);

	//! Left focus, etc...

	//! Don't schedule the hover action any more.
	//!
	//! Overriden in child_elementObj, which invokes this and also
	//! invokes its container unschedule_hover_action().
	virtual void unschedule_hover_action(ONLY IN_THREAD);

	//! If this widget takes action on a hover delay, return the delay.

	//! A widget that implements a hover-triggered action
	//! implements hover_action_delay() together with hover_action()
	//! and hover_cancel().
	//!
	//! Returns 0 if this widget does not have a hover-triggered
	//! action, at the moment, otherwise the delay before the action
	//! takes place.
	//!
	//! The default implementation returns the tooltip delay
	//! configuration setting if this widget has a tooltip factory
	//! installed, else returns 0. Overridden in a list container to
	//! check if the list item has a submenu, returning 0 if not, otherwise
	//! this hover action delay implements opening a submenu.

	virtual std::chrono::milliseconds hover_action_delay(ONLY IN_THREAD);

	//! Element for installed hover action

	//! Return this element by default. Overridden by combo-box containers.
	//! Combo-boxes attach the popups to their containers, and the
	//! hover actions (tooltips) are attached to the combo-box current
	//! elements

	virtual element_impl hover_element_impl();

	//! The pointer has hovered over this element, so do something.

	//! A widget that implements a hover-triggered action
	//! implements hover_action_delay() together with hover_action()
	//! and hover_cancel().
	//!
	//! Only gets invoked when hover_action_delay() returned a non-0
	//! interval, which is now elapsed.
	//!
	//! The default implementation shows the tooltip. Overriden in a list
	//! container to show the submenu when hovering over a menu item with
	//! a submenu.
	virtual void hover_action(ONLY IN_THREAD);

	//! Create and show the tooltip

	//! Always returns a non-null popup. By default it goes into
	//! data.attached_popup. editor_implObj also uses it to create its
	//! own direction popup that it manages.

	popupptr show_tooltip(ONLY IN_THREAD,
			      const functionref<void (THREAD_CALLBACK,
						      const tooltip_factory &)>
			      &tooltip_factory);

	//! Hide any popups

	//! Gets invoked whenever this element loses keyboard or pointer
	//! focus, or is no longer visible. Does nothing. Overridden
	//! in editor_implObj to hide the text direction tooltip, if it's
	//! displayed.
	virtual void hide_popups(ONLY IN_THREAD) LIBCXX_PUBLIC;

	//! Hide the tooltip.

	//! A widget that implements a hover-triggered action
	//! implements hover_action_delay() together with hover_action()
	//! and hover_cancel().
	//!
	//! Gets invoked by unschedule_hover_action().
	//!
	//! The default implementation removes the tooltip, if one is
	//! currently shown.
	//! interval, which is now elapsed.
	void hide_tooltip(ONLY IN_THREAD) LIBCXX_HIDDEN;

	//! Whether there's a tooltip attached to this element.

	bool has_tooltip_popup(ONLY IN_THREAD) const LIBCXX_HIDDEN;

	//! Compute pointer event as pseudo tooltip anchor element

	//! Popups attach to the element's coordinates. Tooltips use the
	//! popup attachment framework, but get anchored to the pointer
	//! position.
	//!
	//! This takes the last reported pointer position and constructs
	//! a pseudo rectangle for an element 0x0 at the pointer coordinates.
	std::tuple<rectangle, attached_to>
	tooltip_pseudo_rectangle(ONLY IN_THREAD,
				 const const_tooltip_appearance &appearance)
		LIBCXX_HIDDEN;

	//! Text direction in this element

	//! Used to position the tooltip to the left or the right of the
	//! pointer position.
	//!
	//! Returns the default paragraph embedding level. Overriden in
	//! labels and input fields.
	virtual bidi direction(ONLY IN_THREAD);

 private:
	//! Schedule a timer to check if the tooltip needs to be shown.
	void schedule_hover_timer(ONLY IN_THREAD,
				    tick_clock_t::time_point now)
		LIBCXX_HIDDEN;

	//! Check if the tooltip needs to be shown.
	void check_hover_timer(ONLY IN_THREAD)
		LIBCXX_HIDDEN;

	//! The most recent keyboard_focus() reported
	focus_change most_recent_keyboard_focus_change_thread_only=focus_change::lost;

	//! The most recent pointer_focus() reported
	focus_change most_recent_pointer_focus_change_thread_only=focus_change::lost;

 public:

	THREAD_DATA_ONLY(most_recent_keyboard_focus_change);
	THREAD_DATA_ONLY(most_recent_pointer_focus_change);

	//! The window lost or gained input focus

	//! The default implementation does nothing.

	virtual void window_focus_change(ONLY IN_THREAD, bool flag);

	//! Whether this widget currently has keyboard input focus.

	//! Returns true if most_recent_keyboard_focus_change was anything
	//! other than focus_change::lost or focus_change::child_lost

	bool current_keyboard_focus(ONLY IN_THREAD);

	//! Whether this widget currently has pointer input focus.

	//! Returns true if most_recent_pointer_focus_change was anything
	//! other than focus_change::lost or focus_change::child_lost

	bool current_pointer_focus(ONLY IN_THREAD);

	//! Make sure that the given rectangle is visible.

	//! The default implementation does nothing.
	//!
	//! child_elementObj forwards this call to its container.
	//!
	//! The container's layout manager, by default, adds this element's
	//! current_position, and forward it to the container's widget.
	//!
	//! The \ref peepholeObj::layoutmanager_implObj "peephole layout manager"
	//! makes sure that its child element's specified position will be
	//! scrolled into view.

	virtual void ensure_visibility(ONLY IN_THREAD, const rectangle &);

	//! Ensure visibility of this entire widget.

	//! Constructs a rectangle for this entire widget's current
	//! size and invokes ensure_visibility().
	void ensure_entire_visibility(ONLY IN_THREAD);

	//! Process pasted text.

	//! Returns true if this widget processed the pasted text.
	//! The default elementation returns false.

	virtual bool pasted(ONLY IN_THREAD,
			    const std::u32string_view &str);

	//! Whether this widget is capable of receiving a selection()

	//! The default implementation returns false. Overridden by the editor
	//! implementation element, returning true.

	virtual bool selection_can_be_received();

	//! Return the default clipboard for copy/cut/paste selection.

	//! This reaches into the default theme object, in order to get it.

	std::string default_cut_paste_selection() const;
	//! Whether this widget has something to copy or cut, or do it.

	//! The default implementation returns false.
	//! Overridden in the editor implementation object.
	virtual bool cut_or_copy_selection(cut_or_copy_op, xcb_atom_t);

	//! Whether this widget has something to copy or cut, or do it.

	//! The default implementation returns false.
	//! Overridden in the editor implementation object.
	virtual bool cut_or_copy_selection(ONLY IN_THREAD,
					   cut_or_copy_op, xcb_atom_t);

	//! Creating a focusable widget in this container.

	//! The default implementation does nothing. child_elementObj also
	//! forwards it to its parent container.

	virtual void creating_focusable_element();

	//! Grab the pointer for this widget.

	//! Returns a mcguffin if the pointer is not already grabbed by another
	//! element, and the grab succeeds.

	grabbed_pointerptr grab_pointer(ONLY IN_THREAD);

	//! The the mouse pointer pursor.
	void set_cursor_pointer(ONLY IN_THREAD, const cursor_pointer &);

	//! Remove the mouse pointer cursor,
	void remove_cursor_pointer(ONLY IN_THREAD);

	//! Retrieve the cursor pointer for this widget.

	//! Overridden in child_elementObj. If this widget does not have a
	//! set cursor pointer, call get_container_cursor_pointer().

	virtual cursor_pointerptr get_cursor_pointer(ONLY IN_THREAD);

	//! Convenient shortcut
	void exception_message(const exception &);

	//! Convenient shortcut
	void stop_message(const text_param &);

	//! Name of this element, for debugging purposes.
	std::string element_name();

	//! Recursively build this element's name.
	virtual void element_name(std::ostream &);

	//! Find an widget that will accept something dropped into it.

	//! find_acceptable_drop() gets called for the widget under the current
	//! drag position.
	//!
	//! The default implementation returns false.
	//!
	//! child_elementObj forwards the call to its container's widget.
	//!
	//! If a widget accepts this drop, find_acceptable_drop()
	//! returns true, with accepting_element set to the accepting widget.
	//!
	//! Otherwise the passed-in accepting_element should not be modified.

	virtual bool find_acceptable_drop(ONLY IN_THREAD,
					  ref<implObj> &accepting_element,
					  const source_dnd_formats_t
					  &source_formats,
					  xcb_timestamp_t timestamp);

	//! Update dragging location.

	//! If accepts_drop(), the accepting element's dragging_location()
	//! gets called.
	virtual void dragging_location(ONLY IN_THREAD, coord_t x, coord_t y,
				       xcb_timestamp_t timestamp);

	//! Drop the dragged content.

	virtual current_selection_handlerptr drop(ONLY IN_THREAD,
						  xcb_atom_t &type,
						  const ref<obj>
						  &finish_mcguffin);

	//! Process a response to my drag messages (from the drop target)

	virtual bool drag_response(ONLY IN_THREAD,
				   const xcb_client_message_event_t *event);

	//! Update the attached_popup.

	//! Invoked from initialize(), process_updated_position(), and
	//! absolute_location_updated() in order to keep the attached popup
	//! positioned accordingly.
	virtual void update_attachedto_info(ONLY IN_THREAD);

	//! Save the element's status

	//! The default implementation forwards this call for_each_child().
	virtual void save(ONLY IN_THREAD);
};

LIBCXXW_NAMESPACE_END

#endif
