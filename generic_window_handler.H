/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef generic_window_handler_h
#define generic_window_handler_h

#include "window_handler.H"
#include "generic_window.H"
#include "shared_handler_datafwd.H"
#include "drawable.H"
#include "picture.H"
#include "iconfwd.H"
#include "background_colorfwd.H"
#include "connection_threadfwd.H"
#include "container_elementfwd.H"
#include "grabbed_pointerfwd.H"
#include "cursor_pointerfwd.H"

#include "x/w/pictformatfwd.H"
#include "x/w/types.H"
#include "x/w/focusablefwd.H"
#include "x/w/busy.H"
#include "focus/focusablefwd.H"
#include "fonts/current_fontcollectionfwd.H"
#include "x/w/fontfwd.H"
#include <string_view>
#include <courier-unicode.h>
#include <list>
#include <x/weakptr.H>
#include <x/mpobj.H>
#include <xcb/xcb_icccm.h>
#include <x/functional.H>

#include <chrono>
#include <optional>

LIBCXXW_NAMESPACE_START

//! Common logic for generic input/output windows.

//! Note that this class doesn't handle xcb_create_window and
//! xcb_destroy_window. An implementation class that holds a reference on
//! this handler is responsible for that.
//!
//! This is an (indirect) superclass of main windows and popups.
//! main_windowObj::handlerObj and popupObj::handlerObj does not
//! inherit from this handlerObj directly, but from its resourceObj.
//!
//! This is because the handler needs to construct some resources that require
//! the handlerObj to already be constructed. This is done by having
//! resourcesObj handle the initialization of the resources after the
//! handlerObj is constructed, and that's what gets inheried from.

class LIBCXX_HIDDEN generic_windowObj::handlerObj

// The window handler
	: public window_handlerObj,

// The window's drawable
	  public drawableObj::implObj,

// The picture for the window
	  public pictureObj::implObj::fromDrawableObj,

// Used by paste() code to convert pasted text to unicode
	  public unicode::iconvert::tou,

// The container
	  public container_elementObj<elementObj::implObj> {

	//! Current event mask.
	xcb_event_mask_t current_events_thread_only;

	coord_t root_x_thread_only; //!< x coordinate on the root window
	coord_t root_y_thread_only; //!< y coordinate on the root window

 public:
	class resourcesObj;

	THREAD_DATA_ONLY(root_x);
	THREAD_DATA_ONLY(root_y);

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

	//! Keep track of opened popups and shortcuts.

	const shared_handler_data handler_data;

	//! Return the popup's parent's main window.

	//! Implemented by returning this. Overridden in popup_handler.
	//!
	//! Because popupObj::handlerObj maintains a weak reference on its
	//! parent, it's possible that a null pointer gets returned.

	virtual ptr<handlerObj> get_popup_parent(IN_THREAD_ONLY);
 private:
	//! Default background color name
	const color_arg original_background_color;

	//! Whether this window is currently receiving input focus

	bool has_focus_thread_only=false;

	//! Whether this window has been exposed
	bool has_exposed_thread_only=false;

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

	//! The current pointer grab.

	weakptr<grabbed_pointerptr> current_pointer_grab_thread_only;
 public:

	//! Attempt to grab a pointer on behalf of an element.

	//! A null grabbing_element indicates a popup grabbing the pointer.
	virtual grabbed_pointerptr grab_pointer(IN_THREAD_ONLY,
						const elementimplptr &);

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

	struct constructor_params {

		//! Parameters for window_handler's constructor.

		window_handlerObj::constructor_params window_handler_params;

		//! The window's pictformat.

		const_pictformat drawable_pictformat;

		//! Normally the top-level window is nesting_level of 0.

		//! But for a combo-box we'll use its parent's nesting as a
		//! base, for optimum recalculation.

		size_t nesting_level;

		//! Background color
		color_arg background_color;
	};

 protected:

	//! Internal constructor
	handlerObj(IN_THREAD_ONLY,
		   const shared_handler_data &handler_data,
		   const constructor_params &params);
 public:
	//! Destructor
	~handlerObj();

	//! Resources implemented in resourcesObj.

	virtual const icon disabled_mask(IN_THREAD_ONLY)=0;

	// Resources implemented in resourcesObj
	virtual const background_color current_background_color(IN_THREAD_ONLY)=0;

	// Resources implemented in resourcesObj
	virtual void set_background_color(IN_THREAD_ONLY,
					  const background_color &)=0;

	// Resources implemented in resourcesObj
	virtual const background_color shaded_color(IN_THREAD_ONLY)=0;

	THREAD_DATA_ONLY(current_events);
	THREAD_DATA_ONLY(has_focus);
	THREAD_DATA_ONLY(frame_extents);
	THREAD_DATA_ONLY(current_pointer_grab);
	THREAD_DATA_ONLY(has_exposed);

	//! Invoked after the handler gets installed.

	//! The handler is now registered with the connection thread.

	void installed(IN_THREAD_ONLY) override;

	////////////////////////////////////////////////////////////////////
	//
	// Inherited from elementObj::implObj

	//! Return myself.
	generic_windowObj::handlerObj &get_window_handler() override;

	//! Return myself.
	const generic_windowObj::handlerObj &get_window_handler() const override;

	//! Construct a draw_info for drawing into the requested viewport.

	//! Give ourselves away as the picture to draw info, and use
	//! our declared background color.

	draw_info &get_draw_info(IN_THREAD_ONLY)
		override;

	//! Override get_absolute_location().

	//! For top level display elements, this is the same as
	//! their alleged current_position.

	rectangle get_absolute_location(IN_THREAD_ONLY) override;

	//! Translate coordinates to screen coordinates.
	void get_absolute_location_on_screen(IN_THREAD_ONLY, rectangle &r);

	//! Take local x/y coordinates, add root_[xy].
	void add_root_xy(IN_THREAD_ONLY, coord_t &x, coord_t &y);

	//! Take root x/y coordinates, subtract root_[xy].
	void subtract_root_xy(IN_THREAD_ONLY, coord_t &x, coord_t &y);

	//! Remove our own background color.

	//! For top level windows, this simply sets their background color
	//! to the default background color.

	void remove_background_color(IN_THREAD_ONLY) override;

	void current_position_updated(IN_THREAD_ONLY) override;

	//! Process TAB/Shift TAB, do move the input focus.

	bool process_key_event(IN_THREAD_ONLY, const key_event &ke) override;

	//! Set default input focus.

	//! If no element currently has keyboard focus, set it to the first
	//! focusable field. Returns true if a display element already has
	//! input focus, or if one was found by set_default_focus().

	bool set_default_focus(IN_THREAD_ONLY);

	//! Set focusable element in the window.

	void get_focus_first(IN_THREAD_ONLY, const focusable &f) override;

	//! Override focusable_initialized

	//! The default implementation does nothing.

	void focusable_initialized(IN_THREAD_ONLY,
				   focusableImplObj &fimpl) override;

 private:
	//! Most recent popup where the pointer motion event went to.
	weakptr<ptr<handlerObj>> most_recent_popup_with_pointer_thread_only;

	//! Most recent element where the pointer motion event went to.

	ptr<elementObj::implObj> most_recent_element_with_pointer_thread_only;

 public:
	THREAD_DATA_ONLY(most_recent_popup_with_pointer);
	THREAD_DATA_ONLY(most_recent_element_with_pointer);

	//! Process pointer motion event.

	void pointer_motion_event(IN_THREAD_ONLY,
				  const xcb_motion_notify_event_t *)
		override;

	//! Enter event.
	void enter_notify_event(IN_THREAD_ONLY,
				const xcb_enter_notify_event_t *) override;

	//! Leave event.
	void leave_notify_event(IN_THREAD_ONLY,
				const xcb_enter_notify_event_t *) override;

	//! Focus change event
	void focus_change_event(IN_THREAD_ONLY, bool) override;

 private:

	//! Find an element for the reported pointer X and Y coordinates.

	//! And if it's a different element now, report_pointer_focus()
	//! changes, accordingly.
	//!
	//! Returns which top level handler the pointer was reported to
	//! (could be another one, that's actively grabbed the pointer).

	ref<handlerObj> report_pointer_xy(IN_THREAD_ONLY,
					  motion_event &me,
					  bool was_grabbed);

	//! Use grab_locked() for was_grabbed in the overloaded method.

	ref<handlerObj> report_pointer_xy(IN_THREAD_ONLY,
					  motion_event &me);

	//! Decided that this is the handler that the pointer should be reported to.
	void report_pointer_xy_to_this_handler(IN_THREAD_ONLY,
					       const grabbed_pointerptr &pg,
					       motion_event &me,
					       bool was_grabbed);

	//! This is the element that should be receiving pointer events.

	void set_element_with_pointer(IN_THREAD_ONLY,
				      const ref<elementObj::implObj> &e);

 public:
	//! An element is being removed.

	//! This takes care of making sure the element is not pointed to from
	//! most_recent_element_with_pointer.
	void removing_element(IN_THREAD_ONLY, const ref<elementObj::implObj> &);
 private:

	//! Remove pointer reporting

	//! The element with the pointer focus is being removed, or the
	//! pointer has left the window. Report lossage of pointer focus.

	void pointer_focus_lost(IN_THREAD_ONLY);
 public:

	////////////////////////////////////////////////////////////////////
	//
	// Inherited from window_handler

	//! Implement the exposure event.

	void exposure_event(IN_THREAD_ONLY,
			    const rectangle_set &areas) override;

	//! Theme update notification.
	void theme_updated_event(IN_THREAD_ONLY) override;

	//! Key press event
	void key_press_event(IN_THREAD_ONLY,
			     const xcb_key_press_event_t *event,
			     uint16_t sequencehi) override;

	//! Key release event
	void key_release_event(IN_THREAD_ONLY,
			       const xcb_key_release_event_t *event,
			       uint16_t sequencehi) override;
	//! Button press event
	void button_press_event(IN_THREAD_ONLY,
				const xcb_button_press_event_t *event) override;

 private:
	//! The last button press event timestamp.

	std::optional<std::chrono::time_point<std::chrono::steady_clock>
		      > previous_click_time;

	//! Counts double, triple, etc... clicks
	unsigned click_count=0;

 public:
	//! Button release event
	void button_release_event(IN_THREAD_ONLY,
				  const xcb_button_release_event_t *event) override;

	//! A container for the busy mcguffin.

	typedef mpobj<weakptr<ptr<obj>>> busy_mcguffin_t;

	//! Construct the busy mcguffin.

	ref<obj> get_busy_mcguffin();
	//! Busy mcguffin.

	busy_mcguffin_t busy_mcguffin;

	//! Whether we're busy (ignore keyboard or button press events)

	bool is_busy();

	//! An element's button_press_event() wishes to grab the pointer.

	void grab(IN_THREAD_ONLY, const ref<elementObj::implObj> &e);

	friend class shared_handler_dataObj;
 protected:

	//! Forward key event to the X Input Method server.

	//! If XIM not available, call handle_key_event().
	void forward_key_event(IN_THREAD_ONLY,
			       const xcb_key_release_event_t *event,
			       uint16_t sequencehi,
			       bool keypress);

	//! Process a key press or release event.
	bool handle_key_event(IN_THREAD_ONLY,
			      const xcb_key_release_event_t *event,
			      bool keypress) override;

	//! Process a button press or release event.

	//! Use grab_locked() for was_grabbed in the overloaded method.

	void do_button_event(IN_THREAD_ONLY,
			     const xcb_button_release_event_t *event,
			     bool buttonpress);

	//! Process a button press or release event.

	//! Calls report_pointer_xy() to find the element under cursor,
	//! and constructs a button_event.

	void do_button_event(IN_THREAD_ONLY,
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

	virtual void do_button_event(IN_THREAD_ONLY,
				     const xcb_button_release_event_t *event,
				     const button_event &be,
				     const motion_event &me);

	//! The top level display element cannot grab. Throw an exception.

	void grab(IN_THREAD_ONLY) override;

 public:
	//! Do not redraw after the window's visibility is updated.

	//! Instead, we redraw in response to exposure events.

	void draw_child_elements_after_visibility_updated(IN_THREAD_ONLY,
							  bool flag) override;

	//! Setting the inherited_visibility flag.

	//! This gets translated to mapping/unmapping the window.

	void set_inherited_visibility(IN_THREAD_ONLY,
				      inherited_visibility_info &info) override;

	//! This window, and all its elements, have been mapped.

	void mapped(IN_THREAD_ONLY);

	//! Whether this window is mapped.

	bool is_mapped(IN_THREAD_ONLY) const
	{
		return data(IN_THREAD).inherited_visibility;
	}

	//! Update the displayed window's element's current_position

	void configure_notify(IN_THREAD_ONLY,
			      const rectangle &) override;

	//! Update frame extents

	//! Invoked in response to the _NET_FRAME_EXTENTS property getting
	//! updated.

	void update_frame_extents(IN_THREAD_ONLY);

	//! Frame extents have changed.

	//! The default implementation does nothing.

	virtual void frame_extents_updated(IN_THREAD_ONLY);

	//! Set extended window manage hints for this window's type.

	void set_window_type(const std::string &s);

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

	//! Create a non-theme font for a display element in this window.

	//! Creates a \ref current_fontcollection "current font collection".
	//!
	//! Even though it's a non-theme font, the font's size still depends on
	//! the theme's scale, so it is still a theme-dependent font. Everything
	//! is a theme-dependent font.
	//!
	//! These fonts get cached in screen's custom_screen_font_cache.

	current_fontcollection create_font(const font &props);

	//! Create a theme font for a display element in this window.

	current_fontcollection create_theme_font(const std::string_view &font);

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

	std::list<ref<focusableImplObj>> focusable_fields_thread_only;

	//! Current field that's receiving input focus.
	ptr<focusableImplObj> most_recent_keyboard_focus_thread_only;

 public:

	THREAD_DATA_ONLY(focusable_fields);
	THREAD_DATA_ONLY(most_recent_keyboard_focus);

	//! Move the current keyboard focus to another element.
	void set_keyboard_focus_to(IN_THREAD_ONLY, const focusable_impl &f);

	//! No element has keyboard focus any more.
	void unset_keyboard_focus(IN_THREAD_ONLY);

	//! Set input focus to this window
	virtual void set_input_focus(IN_THREAD_ONLY);

	//! Paste a string from a clipboard.

	//! Should only be called by an element that's receiving keyboard
	//! input focus. Results in that element's pasted() getting invoked,
	//! piecemeal, with the pasted text.

	void paste(IN_THREAD_ONLY, xcb_atom_t clipboard,
		   xcb_timestamp_t timestamp);

 private:

	////////////////////////////////////////////////////////////////////
	//
	// Inherited from window_handlerObj. Implements its conversion
	// callbacks to convert the pasted text to unicode.

	//! Should this data be converted?

	//! Recognize STRING and UTF8_STRING atoms, that start the conversion.

	bool begin_converted_data(IN_THREAD_ONLY, xcb_atom_t type,
				  xcb_timestamp_t timestamp) override;

	//! Receive converted data.

	void converted_data(IN_THREAD_ONLY, xcb_atom_t clipboard,
			    xcb_atom_t actual_type,
			    xcb_atom_t format,
			    void *data,
			    size_t size) override;

	//! Whether converted_data was called after begin_converted_data.

	bool received_converted_data;

	//! Converted data has been received.
	void end_converted_data(IN_THREAD_ONLY, xcb_atom_t clipboard,
				xcb_timestamp_t timestamp) override;

	//! Incremental conversion in progress.

	void converting_incrementally(IN_THREAD_ONLY,
				      xcb_atom_t type,
				      xcb_timestamp_t timestamp,
				      uint32_t estimated_size) override;

	//! Incremental conversion in progress.
	bool incremental_conversion_in_progress;

	//! Implement conversion_failed().

	//! This means that paste() failed.

	void conversion_failed(IN_THREAD_ONLY, xcb_atom_t clipboard,
			       xcb_atom_t type,
			       xcb_timestamp_t timestamp) override;

	//! Inherited from unicode::iconvert::tou

	//! Handles the converted_data() as unicode.

	int converted(const char32_t *ptr, size_t cnt) override;

 public:
	//! Pasted text into this window. Forward to pasted().

	//! This is also called to commit an string from the XIM server.

	void pasted_string(IN_THREAD_ONLY,
			   const std::u32string_view &)
		override;

	// Retrieve WM_HINTS, invoke functor to update it.

	typedef void update_wm_hints_t(xcb_icccm_wm_hints_t &);

	template<typename functor_type>
		void update_wm_hints(functor_type &&functor)
	{
		do_update_wm_hints(make_function<update_wm_hints_t>
				   (std::forward<functor_type>(functor)));
	}

	void do_update_wm_hints(const function<update_wm_hints_t> &callback);

 private:
	//! Which cursor pointer we're currently displaying.

	cursor_pointerptr displayed_cursor_pointer;

 public:
	//! Figure out which cursor pointer this window should be displaying.

	//! Set XCB_CW_CURSOR accordingly.
	void update_displayed_cursor_pointer(IN_THREAD_ONLY);

};

LIBCXXW_NAMESPACE_END

#endif
