/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef generic_window_handler_h
#define generic_window_handler_h

#include "window_handler.H"
#include "generic_window.H"
#include "override_redirectfwd.H"
#include "window_handler_constructor_params.H"
#include "shared_handler_datafwd.H"
#include "drawable.H"
#include "picture.H"
#include "gc.H"
#include "x/w/impl/iconfwd.H"
#include "icon_images_set_elementfwd.H"
#include "cursor_pointer_elementfwd.H"
#include "shortcut/installed_shortcutfwd.H"
#include "selection/current_selectionfwd.H"
#include "x/w/impl/focus/delayed_input_focusfwd.H"
#include "x/w/impl/selection/current_selection_handlerfwd.H"
#include "x/w/impl/background_colorfwd.H"
#include "x/w/impl/background_color_elementfwd.H"
#include "x/w/connection_threadfwd.H"
#include "x/w/impl/container_elementfwd.H"
#include "x/w/impl/grabbed_pointerfwd.H"

#include "x/w/pictformatfwd.H"
#include "x/w/types.H"
#include "x/w/focusablefwd.H"
#include "x/w/busy.H"
#include "x/w/rgb.H"
#include "x/w/main_windowfwd.H"
#include "x/w/generic_window_appearancefwd.H"
#include "x/w/impl/focus/focusablefwd.H"
#include "x/w/impl/dnd.H"
#include "x/w/fontfwd.H"
#include "size_hintsfwd.H"
#include <string_view>
#include <courier-unicode.h>
#include <list>
#include <x/weakptr.H>
#include <x/weaklist.H>
#include <x/mpobj.H>
#include <xcb/xcb_icccm.h>
#include <x/functional.H>

#include <chrono>
#include <optional>
#include <variant>

LIBCXXW_NAMESPACE_START

struct background_color_tag;
struct shaded_color_tag;
struct disabled_mask_tag;

//! Tag: window is not being resized

struct LIBCXX_HIDDEN not_resizing {};

//! Indicates window resizing status.

//! It's either not_resizing, or we're waiting for the window manager to
//! resize it, but we'll give up hoping after a timeout expires and we'll
//! go ahead and finish our own processing, at that time.

typedef std::variant<not_resizing,
		     tick_clock_t::time_point> resizing_t;

//! Common logic for generic input/output windows.

//! Note that this class doesn't handle xcb_create_window and
//! xcb_destroy_window. An implementation class that holds a reference on
//! this handler is responsible for that.

class generic_windowObj::handlerObj

// The window handler
	: public window_handlerObj,

// The window's drawable
	  public drawableObj::implObj,

// The picture for the window
	  public pictureObj::implObj::fromDrawableObj,
// The graphic context for the window

	  public gcObj::handlerObj,

// The container
	  public background_color_elementObj<icon_images_set_elementObj<
					     cursor_pointer_elementObj<
						     container_elementObj
						     <elementObj::implObj>>,
						     disabled_mask_tag>,
					     background_color_tag,
					     shaded_color_tag> {

	//! Alias.

	typedef background_color_elementObj<
		icon_images_set_elementObj<
			cursor_pointer_elementObj<
				container_elementObj
				<elementObj::implObj>>,
			disabled_mask_tag>,
		background_color_tag,
		shaded_color_tag
		> superclass_t;

	//! Current event mask.
	xcb_event_mask_t current_events_thread_only;

	coord_t root_x_thread_only; //!< x coordinate on the root window
	coord_t root_y_thread_only; //!< y coordinate on the root window

 public:

	// Resolve some ambiguities.

	using window_handlerObj::conn;
	using window_handlerObj::thread;

	//! x & y coordinates on the root window.

	//! This is a mutex-protected copy of root_x and root_y. root_x and
	//! root_y are accessible ONLY IN_THREAD, and the connection thread
	//! uses them, for speed, thusly no need to avoid locking. The
	//! connection thread, upon received a ConfigureNotify, sets both
	//! root_x, root_y, and root_xy.

	mpobj<std::tuple<coord_t, coord_t>> root_xy;

 private:

	//! What we will put into WM_CLASS
	std::string wm_class_instance_thread_only;

	//! What we will put into WM_CLASS
	std::string wm_class_resource_thread_only;

 public:
	//! The default value for the instance
	virtual const char *default_wm_class_instance() const=0;

	//! The default value for the resource.

	//! The default implementation grabs exename.

	virtual std::string default_wm_class_resource(ONLY IN_THREAD);

	class resourcesObj;

	THREAD_DATA_ONLY(root_x);
	THREAD_DATA_ONLY(root_y);
	THREAD_DATA_ONLY(wm_class_instance);
	THREAD_DATA_ONLY(wm_class_resource);

	//! Current position and dimensions

	//! This is a mutex-protected object. ConfigureNotify event saves
	//! a copy of the new window location and size here, before
	//! invoking the element superclass's update_current_position().
	//!
	//! get_width() and get_height(), inherited from drawableObj read this.
	//! The mutex protection allows them to be used outside of the
	//! connection thread, but this means that this object must be
	//! mutable, to be usable by the overridden const methods.

	mutable mpobj<rectangle> current_position;

 private:
	//! Whether we expect the window manager to resize us, shortly.

	//! update_resizing_timeout() sets a time_point if it believes that
	//! we will be resized soon, but we'll give up this hope at this
	//! time.
	resizing_t resizing_thread_only=not_resizing{};

	//! Window size and position stabilized callbacks.

	std::vector<stabilized_callback_t> stabilized_callbacks_thread_only;

	//! Backing store where the window's contents get built first,

	//! Then copied to the window's drawable in flush_redrawn-areas.

	pixmap window_pixmap_thread_only;

	//! window_pixmap's picture
	picture window_picture_thread_only;

	//! Which parts of the window_pixmap are dirty.

	rectarea window_drawnarea_thread_only;

	//! Update window_pixmap to reflect the window's current size.

	//! Invoked from do_process_configure_notify, unless the window
	//! has not been exposed yet. Then invoked from
	//! process_collected_exposures(), in order to catch up because of
	//! that.
	void update_window_pixmap_and_picture(ONLY IN_THREAD,
					      const rectangle &);

	//! Copy window_drawnarea to the window's drawable.
	void flush_redrawn_areas(ONLY IN_THREAD) override;

 public:
	THREAD_DATA_ONLY(resizing);

	//! Whether this window is being resized

	//! Either by a window, or by ourselves (this is an override-redirected
	//! popup).

	inline bool is_resizing(ONLY IN_THREAD)
	{
		return !std::holds_alternative<not_resizing>
			(resizing(IN_THREAD));
	}
	THREAD_DATA_ONLY(stabilized_callbacks);

	THREAD_DATA_ONLY(window_pixmap);
	THREAD_DATA_ONLY(window_picture);
	THREAD_DATA_ONLY(window_drawnarea);

	//! Make sure that the window_pixmap has been resized.

	//! Invoked from draw_to_window_picture. Certain sequences of events
	//! may result in window_pixmap not yet been resized to accomodate
	//! a larger window. But we're now drawing in it, so we need to make
	//! sure it is.

	void drawing_to_window_picture(ONLY IN_THREAD,
				       const rectangle &rect);

	//! Copy window_drawnarea to the window's drawable.

	void flush_redrawn_areas(ONLY IN_THREAD,
				 rectarea &redrawn);

	//! Direct scroll

	//! Used to scroll parts of window directly, after it was
	//! determine that this area is eligible to be scrolled.
	//!
	//! Scrolls the underlying pixels in the window, as well as the
	//! backing window_pixmap.

	void scroll_window_pixmap(ONLY IN_THREAD,
				  const rectangle &r,
				  coord_t scrolled_to_x,
				  coord_t scrolled_to_y);

	//! Determine whether we expect to be resized by the window manager.

	//! If our current size is outside of our minimum or maximum size we
	//! expect our window manager to resize it. Set the resizing flag
	//! if that's the case, and compute the resizing_timeout.
	//!
	//! resize_pending() returns true if resizing, but if resizing_timeout
	//! is reached the resizing flag gets cleared, we gave up hope.
	//! We postpone some processing pending resizing by the window manager,
	//! in order to save some work that gets mooted by the resize, so if
	//! the respected resize does not come we will proceed and do what we
	//! wanted.
	virtual void update_resizing_timeout(ONLY IN_THREAD);

	//! Invoke stabilized_callbacks(), if the conditions are right.

	void invoke_stabilized(ONLY IN_THREAD);

	//! Keep track of opened popups and shortcuts.

	const shared_handler_data handler_data;

	//! Local shortcuts, in this window only

	mpobj<shortcut_lookup_t> local_shortcuts;

	//! Container that keeps track of immediate popups

	typedef weaklist<handlerObj> my_popups_t;

	//! Besides shared_handler_data, need to know all direct popups from me.

	const my_popups_t my_popups;

	//! Raise this window.

	//! Recursively raise my popups, too.

	void raise(ONLY IN_THREAD);

	//! Lower this window

	//! Recursively raise my popups, too.

	void lower(ONLY IN_THREAD);

	//! Return the popup's parent's main window.

	//! Implemented by returning this. Overridden in popup_handler.
	//!
	//! Because popupObj::handlerObj maintains a weak reference on its
	//! parent, it's possible that a null pointer gets returned.

	virtual ptr<handlerObj> get_popup_parent(ONLY IN_THREAD);
 private:
	//! Default background color name
	const color_arg original_background_color;

	//! Whether this window is currently receiving input focus

	bool has_focus_thread_only=false;

	//! Whether this window has been exposed
	bool has_exposed_thread_only=false;

	//! Whether this window has been mapped
	bool has_mapped_thread_only=false;

	//! Window manager's frame size and screen geometry.
	struct frame_extents_t {

		//! My workarea.
		rectangle workarea;

		dim_t left,		//!< Window manager frame's size
			right,		//!< Window manager frame's size
			top,		//!< Window manager frame's size
			bottom;		//!< Window manager frame's size

		//! Constructor
		frame_extents_t(const rectangle &workarea);

		//! Comparison operator.
		bool operator==(const frame_extents_t &o);
	};

	//! The current window frame size and screen geometry.

	frame_extents_t frame_extents_thread_only;

	//! The current theme in effect for this window.
	const_defaulttheme current_theme_thread_only;

	//! The current pointer grab.

	weakptr<grabbed_pointerptr> current_pointer_grab_thread_only;

	//! The widget that has requested input focus.

	weakptr<delayed_input_focusptr> scheduled_input_focus_thread_only;
 public:

	//! Determine which element is receiving pointer events now.

	//! This can be current_pointer_grab(), or
	//! most_recent_popup_with_pointer(),
	//! most_recent_element_with_pointer().

	element_implptr current_pointer_event_destination(ONLY IN_THREAD);

	//! Attempt to grab a pointer on behalf of an element.

	//! A null grabbing_element indicates a popup grabbing the pointer.
	virtual grabbed_pointerptr grab_pointer(ONLY IN_THREAD,
						const element_implptr &);

	//! Implement is_pointer_actively_grabbed()

	bool is_pointer_actively_grabbed(ONLY IN_THREAD) override;

	//! What I want for my initial event mask.

	static constexpr xcb_event_mask_t initial_event_mask()
	{
		return (xcb_event_mask_t)(XCB_EVENT_MASK_EXPOSURE |
					  XCB_EVENT_MASK_KEY_PRESS |
					  XCB_EVENT_MASK_KEY_RELEASE |
					  XCB_EVENT_MASK_BUTTON_PRESS |
					  XCB_EVENT_MASK_BUTTON_RELEASE |
					  XCB_EVENT_MASK_ENTER_WINDOW |
					  XCB_EVENT_MASK_LEAVE_WINDOW |
					  XCB_EVENT_MASK_POINTER_MOTION |
					  XCB_EVENT_MASK_VISIBILITY_CHANGE |
					  XCB_EVENT_MASK_PROPERTY_CHANGE |
					  XCB_EVENT_MASK_STRUCTURE_NOTIFY |
					  XCB_EVENT_MASK_FOCUS_CHANGE);
	}

	//! What my constructor needs

	struct extra_constructor_params {

		//! Parameters for window_handler's constructor.

		window_handlerObj::constructor_params window_handler_params;

		//! The window's pictformat.

		const_pictformat drawable_pictformat;

		//! Normally the top-level window is nesting_level of 0.

		//! But for a combo-box we'll use its parent's nesting as a
		//! base, for optimum recalculation.

		size_t nesting_level;

		//! Background color
		color_arg background_color_arg;

		//! The instantiated background_color
		background_color background_color_obj;

		//! Appearance
		const const_generic_window_appearance &appearance;
	};

	//! Public constructor
	handlerObj(const generic_window_handler_constructor_params &params);
 protected:

	//! Internal constructor
	handlerObj(ONLY IN_THREAD,
		   const shared_handler_data &handler_data,
		   const extra_constructor_params &params);
 public:
	//! Destructor
	~handlerObj();

	//! Return the top level main window.
	virtual main_windowptr get_main_window()=0;

	//! Return the current background color.
	background_color current_background_color(ONLY IN_THREAD) override;

	//! Implement set_background_color() for the top level window.
	void set_background_color(ONLY IN_THREAD,
				  const background_color &) override;

	//! Override background_color_changed().
	void background_color_changed(ONLY IN_THREAD) override;

	//! Override process_updated_position().
	void process_updated_position(ONLY IN_THREAD) override;

	//! Override get_parent_element_impl();

	elementObj::implObj *get_parent_element_impl() const override;

 private:
	void update_background_pixel(ONLY IN_THREAD,
				     const background_color &);
 public:
	//! Return the color for drawing a shade over a disabled window.
	const background_color shaded_color(ONLY IN_THREAD);

	//! Return the icon for the disabled element's overlay mask.

	const icon disabled_mask(ONLY IN_THREAD);

	THREAD_DATA_ONLY(current_events);
	THREAD_DATA_ONLY(has_focus);
	THREAD_DATA_ONLY(frame_extents);
	THREAD_DATA_ONLY(current_pointer_grab);
	THREAD_DATA_ONLY(scheduled_input_focus);

	THREAD_DATA_ONLY(has_exposed);
	THREAD_DATA_ONLY(has_mapped);
	THREAD_DATA_ONLY(current_theme);

	//! Invoked after the handler gets installed.

	//! The handler is now registered with the connection thread.

	void installed(ONLY IN_THREAD) override;

	////////////////////////////////////////////////////////////////////
	//
	// Inherited from elementObj::implObj

	//! Implement get_draw_info().

	draw_info &get_draw_info(ONLY IN_THREAD)
		override;

	//! Implement get_draw_info() from scratch.

	//! Use our declared background color.

	draw_info &get_draw_info_from_scratch(ONLY IN_THREAD) override;

	//! Override get_absolute_location().

	//! For top level display elements, this is the same as
	//! their alleged current_position.

	rectangle get_absolute_location(ONLY IN_THREAD) const override;

	//! Translate coordinates to screen coordinates.
	void get_absolute_location_on_screen(ONLY IN_THREAD, rectangle &r)
		const;

	//! Take local x/y coordinates, add root_[xy].
	void add_root_xy(ONLY IN_THREAD, coord_t &x, coord_t &y);

	//! Take root x/y coordinates, subtract root_[xy].
	void subtract_root_xy(ONLY IN_THREAD, coord_t &x, coord_t &y);

	//! Remove our own background color.

	//! For top level windows, this simply sets their background color
	//! to the default background color.

	void remove_background_color(ONLY IN_THREAD) override;

	//! Implement has_own_background_color()

	//! Always returns true

	bool has_own_background_color(ONLY IN_THREAD) override;

	//! Implement current_position_update()

	//! Calls schedule_update_position_processing().

	void current_position_updated(ONLY IN_THREAD) override;

	//! Metrics updated.

	void horizvert_updated(ONLY IN_THREAD) override;

	//! Size hints have been installed.

	//! This is implemented in main_window_handlerObj.
	virtual void install_size_hints(const size_hints &hints);

	//! Size hints have been updated.

	//! This is implemented in main_window_handlerObj.
	virtual void size_hints_updated(ONLY IN_THREAD);

	//! Process TAB/Shift TAB, do move the input focus.

	bool process_key_event(ONLY IN_THREAD, const key_event &ke) override;

private:
	//! process_key_event() searches for a suitable shortcut.
	installed_shortcutptr lookup_shortcut(ONLY IN_THREAD,
					      const key_event &ke);
public:

	//! Set default input focus.

	//! If no element currently has keyboard focus, set it to the first
	//! focusable field. Returns true if a display element already has
	//! input focus, or if one was found by set_default_focus().

	bool set_default_focus(ONLY IN_THREAD, const callback_trigger_t &);

	//! Set focusable element in the window.

	void get_focus_first(ONLY IN_THREAD, const focusable &f) override;

	//! Override focusable_initialized

	//! The default implementation does nothing.

	void focusable_initialized(ONLY IN_THREAD,
				   focusableObj::implObj &fimpl) override;

	//! Check if input focus can be moved to the widget that requested it.

	//! Checks if there's a delayed_input_focus, and whether this
	//! widget can now handle it.
	bool process_focus_updates(ONLY IN_THREAD) override;

 private:
	//! Most recent popup where the pointer motion event went to.
	weakptr<ptr<handlerObj>> most_recent_popup_with_pointer_thread_only;

	//! Most recent element where the pointer motion event went to.

	ptr<elementObj::implObj> most_recent_element_with_pointer_thread_only;

 public:
	THREAD_DATA_ONLY(most_recent_popup_with_pointer);
	THREAD_DATA_ONLY(most_recent_element_with_pointer);

	//! Process pointer motion event.

	void pointer_motion_event(ONLY IN_THREAD,
				  const xcb_motion_notify_event_t *)
		override;

	//! Enter event.
	void enter_notify_event(ONLY IN_THREAD,
				const xcb_enter_notify_event_t *) override;

	//! Leave event.
	void leave_notify_event(ONLY IN_THREAD,
				const xcb_enter_notify_event_t *) override;

	//! Focus change event
	void focus_change_event(ONLY IN_THREAD, bool) override;

 private:

	//! Find an element for the reported pointer X and Y coordinates.

	//! And if it's a different element now, report_pointer_focus()
	//! changes, accordingly.
	//!
	//! Returns which top level handler the pointer was reported to
	//! (could be another one, that's actively grabbed the pointer).

	ref<handlerObj> report_pointer_xy(ONLY IN_THREAD,
					  motion_event &me,
					  bool was_grabbed);

	//! Use grab_locked() for was_grabbed in the overloaded method.

	ref<handlerObj> report_pointer_xy(ONLY IN_THREAD,
					  motion_event &me);

	//! Decided that this is the handler that the pointer should be reported to.
	void report_pointer_xy_to_this_handler(ONLY IN_THREAD,
					       const grabbed_pointerptr &pg,
					       motion_event me,
					       bool was_grabbed);

	//! Find the element under the given (x, y) coordinates.

	//! Returns the element, and adjusts x and y to be relative to the
	//! element's position.

	element_impl find_element_under(ONLY IN_THREAD,
					coord_t &x,
					coord_t &y);

	//! This is the element that should be receiving pointer events.

	void set_element_with_pointer(ONLY IN_THREAD,
				      const ref<elementObj::implObj> &e);

 public:
	//! An element is being removed.

	//! Invoked by removed_from_container().
	//!
	//! This takes care of making sure the element is not pointed to from
	//! most_recent_element_with_pointer.
	void removing_element_from_window(ONLY IN_THREAD,
					  const ref<elementObj::implObj> &);
 private:

	//! Remove pointer reporting

	//! The element with the pointer focus is being removed, or the
	//! pointer has left the window. Report lossage of pointer focus.

	void pointer_focus_lost(ONLY IN_THREAD,
				const callback_trigger_t &trigger);
 public:

	////////////////////////////////////////////////////////////////////
	//
	// Inherited from window_handler

	//! Implement the exposure event.

	void process_collected_exposures(ONLY IN_THREAD) override;

	//! Implement the graphics exposure event.

	void process_collected_graphics_exposures(ONLY IN_THREAD) override;

	//! Theme update notification.
	void theme_updated_event(ONLY IN_THREAD) override;

	//! Override theme_updated
	void theme_updated(ONLY IN_THREAD,
			   const const_defaulttheme &) override;

	//! Key press event
	void key_press_event(ONLY IN_THREAD,
			     const xcb_key_press_event_t *event,
			     uint16_t sequencehi) override;

	//! Key release event
	void key_release_event(ONLY IN_THREAD,
			       const xcb_key_release_event_t *event,
			       uint16_t sequencehi) override;
	//! Button press event
	void button_press_event(ONLY IN_THREAD,
				const xcb_button_press_event_t *event) override;

	//! Update net_user_time property

	void update_user_time(ONLY IN_THREAD);

	//! Update net_user_time property

	void update_user_time(ONLY IN_THREAD, xcb_timestamp_t);

 private:
	//! The last button press event timestamp.

	std::optional<std::chrono::time_point<std::chrono::steady_clock>
		      > previous_click_time;

	//! Counts double, triple, etc... clicks
	unsigned click_count=0;

 public:
	//! Button release event
	void button_release_event(ONLY IN_THREAD,
				  const xcb_button_release_event_t *event) override;

	//! Shade and busy mcguffin objects
	struct busy_mcguffins_s {

		//! The currently-advertised shade mcguffins
		weakptr<ptr<obj>> shade;

		//! All shade mcguffin(s) in existence.

		//! A race condition can destroy and create a new shade
		//! mcguffin before we had the chance to remove the actual
		//! shade from the window.
		//!
		//! Keep track of all shade mcguffins here, to correctly
		//! sequence shading and unshading of the window.
		std::list<ref<obj>> all_shade_mcguffins;

		//! The currently-advertised wait cursor mcguffin.
		weakptr<ptr<obj>> wait_cursor;
	};

	typedef mpobj<busy_mcguffins_s> busy_mcguffins_t;

	//! The busy mcguffins.

	busy_mcguffins_t busy_mcguffins;

	//! Construct a busy mcguffin that shades this window.

	ref<obj> get_shade_busy_mcguffin();

	//! Shade mcguffin has been destroyed

	//! Invoked from the shade mcguffin's destructor. If another
	//! shade mcguffin has not been created yet.

	void shade_mcguffin_destroyed(ONLY IN_THREAD,
				      std::list<ref<obj>>::iterator iterator);

	//! Construct a busy mcguffin that puts up a "wait" cursor.

	ref<obj> get_wait_busy_mcguffin();

	//! Whether we're busy (ignore keyboard or button press events)

	bool is_input_busy();

	//! Whether a shade is drawn.

	bool is_shade_busy();

	//! Whether a wait cursor is used by this window.

	bool is_wait_busy();

	//! An element's button_press_event() wishes to grab the pointer.

	void grab(ONLY IN_THREAD, const ref<elementObj::implObj> &e);

	//! Process XdndEnter from drag source
	void process_drag_enter(ONLY IN_THREAD,
				const xcb_client_message_event_t *event);

	//! Process XdndPosition from drag source
	void process_drag_position(ONLY IN_THREAD,
				   const xcb_client_message_event_t *event);

	//! Process XdndLeave from drag source
	void process_drag_leave(ONLY IN_THREAD,
				const xcb_client_message_event_t *event);

	//! Process XdndDrop from drag source
	void process_drag_drop(ONLY IN_THREAD,
			       const xcb_client_message_event_t *event);

	//! Process XdndStatus and XdndFinish from the drop target.

	void process_drag_response(ONLY IN_THREAD,
				   const xcb_client_message_event_t *event);
	friend class shared_handler_dataObj;
 protected:

	//! If the keyboard is grabbed by a popup, forward it.

	//! Otherwise call forward_key_event_to_xim().

	void forward_key_event(ONLY IN_THREAD,
			       const xcb_key_release_event_t *event,
			       uint16_t sequencehi,
			       bool keypress);

	//! Forward key event to the X Input Method server.

	//! If XIM not available, call handle_key_event().
	void forward_key_event_to_xim(ONLY IN_THREAD,
				      const xcb_key_release_event_t *event,
				      uint16_t sequencehi,
				      bool keypress);

	//! Process a key press or release event.
	bool handle_key_event(ONLY IN_THREAD,
			      const xcb_key_release_event_t *event,
			      bool keypress) override;

	//! Process a key press or release event.
	bool handle_key_event(ONLY IN_THREAD, const key_event &ke);

	//! Process a button press or release event.

	//! Use grab_locked() for was_grabbed in the overloaded method.

	void do_button_event(ONLY IN_THREAD,
			     const xcb_button_release_event_t *event,
			     bool buttonpress);

	//! Process a button press or release event.

	//! Calls report_pointer_xy() to find the element under cursor,
	//! and constructs a button_event.

	void do_button_event(ONLY IN_THREAD,
			     const xcb_button_release_event_t *event,
			     bool buttonpress,
			     bool was_grabbed);


	//! Process the button press or release element

	//! Processes the button_event.
	//!
	//! Finds the display element under the pointer and invokes its
	//! process_button_event(). If the button event does not get processed
	//! and it is a button #1 press, the keyboard input focus gets removed
	//! from the element that currently has the keyboard input focus.

	virtual void do_button_event(ONLY IN_THREAD,
				     const xcb_button_release_event_t *event,
				     const button_event &be,
				     const motion_event &me);

	//! The top level display element cannot grab. Throw an exception.

	void grab(ONLY IN_THREAD) override;

 public:
	//! Do not redraw after the window's visibility is updated.

	//! Instead, we redraw in response to exposure events.

	void draw_child_elements_after_visibility_updated(ONLY IN_THREAD,
							  bool flag) override;

	//! Override update_visibility()

	//! Because top_level_always_visible(), update_visibility() gets
	//! overridden to avoid comparing actual_visibility with
	//! requested_visibility().
	void update_visibility(ONLY IN_THREAD) override;

	void inherited_visibility_updated(ONLY IN_THREAD,
					  inherited_visibility_info &) override;

	void set_inherited_visibility_flag(ONLY IN_THREAD,
					   bool logical_flag,
					   bool reported_flag) override;

 protected:

	//! Map this window.

	virtual void set_inherited_visibility_mapped(ONLY IN_THREAD);

	//! Unmap this window.

	virtual void set_inherited_visibility_unmapped(ONLY IN_THREAD);

 public:
	friend class override_redirectObj;

	//! Implement configure_notify_received().

	//! The default implementation calls do_configure_notify_received().
	//! override_redirectObj overrides it and does nothing, and
	//! calls do_configure_notify_received() from set_popup_position().

	void configure_notify_received(ONLY IN_THREAD, const rectangle &r)
		override;

	//! Implement process_configure_notify().

	//! The default implementation calls do_process_configure_notify().
	//! override_redirectObj overrides it and does nothing, and
	//! calls do_process_configure_notify() from set_popup_position().
	void process_configure_notify(ONLY IN_THREAD) override;

protected:

	//! Actual configure_notify_received() processing.

	void do_configure_notify_received(ONLY IN_THREAD, const rectangle &r);

	//! Actual process_configure_notify() processing.
	void do_process_configure_notify(ONLY IN_THREAD);

public:
	//! Implement process_map_notify_event().
	void process_map_notify_event(ONLY IN_THREAD) override;

	//! Implement process_unmap_notify_event().
	void process_unmap_notify_event(ONLY IN_THREAD) override;

	//! Update frame extents

	//! Invoked in response to the _NET_FRAME_EXTENTS property getting
	//! updated.

	void update_frame_extents(ONLY IN_THREAD);

	//! Frame extents have changed.

	//! The default implementation does nothing.

	virtual void frame_extents_updated(ONLY IN_THREAD);

	//! Set extended window manage hints for this window's type.

	void set_window_type(const std::string_view &s);

	//! Set extended window manage hints for this window's type.
	void set_window_type(ONLY IN_THREAD, const std::string_view &s);

	//! Set extended window manager hints for this window's type.

	//! This is invoked from the constructor, so it's not necessary
	//! to flush the connection, something else will flush it.
	void set_window_type_in_constructor(const std::string_view &);

	//! Set extended window manager hints for this window's state.

	//! This is invoked from the constructor, so it's not necessary
	//! to flush the connection, something else will flush it.
	void set_window_state_in_constructor(const std::string_view &);

	// Inherited from gcObj::handlerObj

	//! Return myself.
	drawableObj::implObj &get_drawable_impl() override;

	//! Return myseltf.
	const drawableObj::implObj &get_drawable_impl() const override;

	////////////////////////////////////////////////////////////////////
	//
	// Inherited from drawableObj::implObj

	//! Return my screen
	screen get_screen() override;

	//! Return my screen
	const_screen get_screen() const override;

	//! Return my width
	dim_t get_width() const override;

	//! Return my height
	dim_t get_height() const override;

	//! Set window title
	void set_window_title(const std::string_view &s);

 private:
	///////////////////////////////////////////////////////////////////
	//

	//! A list of focusable fields in this window.

	//! This points to child elements. This would normally be a circular
	//! reference. However, each focusable implementation object is pointed
	//! to by \ref focusableObj::ownerObj "an owner object", whose
	//! constructor and destructor invoke focusable_initialize() and
	//! focusable_deinitialize(). focusable_deinitialize() takes
	//! care of dropping the reference to the child member.

	std::list<focusable_impl> focusable_fields_thread_only;

	//! Current field that's receiving input focus.
	focusable_implptr most_recent_keyboard_focus_thread_only;

 public:

	THREAD_DATA_ONLY(focusable_fields);
	THREAD_DATA_ONLY(most_recent_keyboard_focus);

	//! Thread-safe container for the most recent autorestorable focusable.

	typedef mpobj<weakptr<focusable_implptr>> autorestorable_focusable_t;

	//! Most recent autorestorable focusable.

	//! Each time keyboard focus gets set to a focus_autorestorable()
	//! display element, it gets stored here.

	autorestorable_focusable_t autorestorable_focusable;

	//! Whether the current selection can be received.

	//! Return the element that can receive the current selection.

	element_implptr element_that_can_receive_selection();

	//! Return the most recent autorestorable focusable.

	//! Locks and recovers a strong reference, if possible.

	focusable_implptr get_autorestorable_focusable();

	//! Move the current keyboard focus to another element.
	void set_keyboard_focus_to(ONLY IN_THREAD, const focusable_impl &f,
				   const callback_trigger_t &trigger);

	//! Cancel any delayed keyboard focus move

	//! If a widget has a pending request to receive keyboard focus,
	//! when it can, cancel the request.
	//!
	//! This could be because another widget can receive keyboard focus
	//! first, so this one's request is moot; or another widget also
	//! wants to receive keyboard focus when it can, so it is now the
	//! most recent widget with a keyboard focus request.
	void remove_delayed_keyboard_focus(ONLY IN_THREAD);

	//! No element has keyboard focus any more.
	void unset_keyboard_focus(ONLY IN_THREAD,
				  const callback_trigger_t &trigger);

	//! Set input focus to this window
	virtual void set_input_focus(ONLY IN_THREAD);

	//! Paste a string from a clipboard.

	//! Should only be called by an element that's receiving keyboard
	//! input focus.
	//!
	//! Installs paste_selected_text as our conversion_handler, which
	//! invokes pasted_string() to pass through the converted selection
	//! piecemeal, with the pasted text.
	//!
	//! We try to convert to a utf8_string, if that fails we'll try
	//! to convert to a string, and treat it as iso-8859-1.

	void receive_selection(ONLY IN_THREAD, xcb_atom_t selection);

	//! Paste a string from a clipboard.

	//! \overload
	void receive_selection(ONLY IN_THREAD, const std::string_view &);

	//! Execute a cut/copy operation.

	//! Forwards this request to autorestorable_focusable.

	bool cut_or_copy_selection(cut_or_copy_op,
				   const std::string_view
				   &selection="SECONDARY");

	//! Execute a cut/copy operation.

	//! \overload
	bool cut_or_copy_selection(ONLY IN_THREAD, cut_or_copy_op,
				   const std::string_view
				   &selection="SECONDARY");

	//! Override property_notify_event().

	//! Calls update_frame_extents() for a net_frame_extents notification.
	//! Otherwise forwards it to the superclass.

	void property_notify_event(ONLY IN_THREAD,
				   const xcb_property_notify_event_t *msg)
		override;

 private:

	//! Which clipboard is in use for the current paste operation.
	xcb_atom_t clipboard_being_pasted_thread_only;

	//! Current paste operation's timestamp.
	xcb_timestamp_t clipboard_paste_timestamp_thread_only;

	//! Currently installed handler for receiving converted selections.

	current_selection_handlerptr conversion_handler_thread_only;

 public:
	THREAD_DATA_ONLY(clipboard_being_pasted);
	THREAD_DATA_ONLY(clipboard_paste_timestamp);
	THREAD_DATA_ONLY(conversion_handler);

 private:
	////////////////////////////////////////////////////////////////////
	//
	// Inherited from window_handlerObj. Implements its conversion
	// callbacks to convert the pasted content.

	//! Should this data be converted?

	//! Gets forwarded to the current conversion_handler.

	bool begin_converted_data(ONLY IN_THREAD, xcb_atom_t type,
				  xcb_timestamp_t timestamp) override;

	//! Receive converted data.

	//! Gets forwarded to the current conversion_handler.
	void converted_data(ONLY IN_THREAD, xcb_atom_t clipboard,
			    xcb_atom_t actual_type,
			    xcb_atom_t format,
			    void *data,
			    size_t size) override;

	//! Converted data has been received.

	//! Gets forwarded to the current conversion_handler.

	void end_converted_data(ONLY IN_THREAD) override;

	//! Implement conversion_failed().

	//! Gets forwarded to the current conversion_handler.

	void conversion_failed(ONLY IN_THREAD, xcb_atom_t type) override;

 public:
	//! Pasted text into this window. Forward to pasted().

	//! paste_selected_text() forwards the converted unicode text to here.
	//! This is also called to commit an string from the XIM server.

	void pasted_string(ONLY IN_THREAD,
			   const std::u32string_view &)
		override;

	//! Callback used by update_wm_hints().
	typedef void update_wm_hints_t(xcb_icccm_wm_hints_t &);

	//! Retrieve WM_HINTS, invoke functor to update it.

	template<typename functor_type>
		void update_wm_hints(functor_type &&functor)
	{
		do_update_wm_hints(make_function<update_wm_hints_t>
				   (std::forward<functor_type>(functor)));
	}

	//! Type-erased update_wm_hints().
	void do_update_wm_hints(const function<update_wm_hints_t> &callback);

	//! Use set_default_wm_hints() to update our WM_HINTS

	void update_wm_hints(ONLY IN_THREAD);

	//! Subclass must specify the default WM_HINTS.

	virtual void set_default_wm_hints(ONLY IN_THREAD,
					  xcb_icccm_wm_hints_t &)=0;
 private:
	//! Which cursor pointer we're currently displaying.

	cursor_pointerptr displayed_cursor_pointer;

 public:
	//! Figure out which cursor pointer this window should be displaying.

	//! Set XCB_CW_CURSOR accordingly.
	void update_displayed_cursor_pointer(ONLY IN_THREAD);

	//! Implement client_message_event.

	//! X drag and drop implementation
	void client_message_event(ONLY IN_THREAD,
				  const xcb_client_message_event_t *event)
		override;

 private:
	//! Drag-and-drop source wants to drop something to us.

	xcb_window_t source_dnd_thread_only=XCB_NONE;

	//! Which DND version the source implements.

	unsigned char source_dnd_version_thread_only=0;

	//! Which formats the source supports.
	source_dnd_formats_t source_dnd_formats_thread_only;

	//! Which element is accepting the drop

	weakptr<element_implptr> dnd_drop_target_thread_only;
 public:
	THREAD_DATA_ONLY(source_dnd);
	THREAD_DATA_ONLY(source_dnd_version);
	THREAD_DATA_ONLY(source_dnd_formats);
	THREAD_DATA_ONLY(dnd_drop_target);

	//! Return the default label font in this window.

	font_arg label_theme_font() const override;

	//! Return the default foreground color for labels in this window.

	color_arg label_theme_color() const override;

	//! The appearance object that created me.
	const const_generic_window_appearance appearance;
};

LIBCXXW_NAMESPACE_END

#endif
