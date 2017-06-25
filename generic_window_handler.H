/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef generic_window_handler_h
#define generic_window_handler_h

#include "window_handler.H"
#include "generic_window.H"
#include "drawable.H"
#include "picture.H"
#include "iconfwd.H"
#include "background_colorfwd.H"
#include "connection_threadfwd.H"
#include "container_elementfwd.H"
#include "hotspotfwd.H"
#include "x/w/pictformatfwd.H"
#include "x/w/types.H"
#include "x/w/focusablefwd.H"
#include "x/w/busy.H"
#include "focus/focusablefwd.H"
#include "fonts/current_fontcollectionfwd.H"
#include "x/w/fontfwd.H"
#include <experimental/string_view>
#include <courier-unicode.h>
#include <list>
#include <x/weakptr.H>
#include <x/mpobj.H>
#include <xcb/xcb_icccm.h>
#include <x/functional.H>

LIBCXXW_NAMESPACE_START

//! Common logic for generic input/output windows.

//! Note that this class doesn't handle xcb_create_window and
//! xcb_destroy_window. An implementation class that holds a reference on
//! this handler is responsible for that.

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

 protected:
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
	//! The mask for drawing disabled elements.

	icon disabled_mask_thread_only;

	//! Window's background color

	background_color current_background_color_thread_only;

	//! Whether this window is currently receiving input focus

	bool has_focus_thread_only=false;

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

 public:

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
	};

	handlerObj(IN_THREAD_ONLY,
		   const screen &me);

 private:
	handlerObj(IN_THREAD_ONLY,
		   const constructor_params &params);
 public:
	//! Destructor
	~handlerObj();

	THREAD_DATA_ONLY(current_events);
	THREAD_DATA_ONLY(disabled_mask);
	THREAD_DATA_ONLY(current_background_color);
	THREAD_DATA_ONLY(has_focus);
	THREAD_DATA_ONLY(frame_extents);

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

	//! Remove our own background color.

	//! For top level windows, this simply sets their background color
	//! to the default background color.

	void remove_background_color(IN_THREAD_ONLY) override;

	//! Use its own background color.

	void set_background_color(IN_THREAD_ONLY,
				  const background_color &) override;

	void current_position_updated(IN_THREAD_ONLY) override;

	//! Process TAB/Shift TAB, do move the input focus.

	bool process_key_event(IN_THREAD_ONLY, const key_event &ke) override;

 private:
	//! Most recent element where the pointer motion event went to.

	ptr<elementObj::implObj> most_recent_element_with_pointer_thread_only;

 public:
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

	void report_pointer_xy(IN_THREAD_ONLY,
			       const input_mask &mask,
			       coord_t x,
			       coord_t y,
			       bool was_grabbed);

	//! Use grab_locked() for was_grabbed in the overloaded method.

	void report_pointer_xy(IN_THREAD_ONLY,
			       const input_mask &mask,
			       coord_t x,
			       coord_t y);

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

	//! Button release event
	void button_release_event(IN_THREAD_ONLY,
				  const xcb_button_release_event_t *event) override;

	//! A container for the busy mcguffin.

	typedef mpobj<weakptr<ptr<obj>>> busy_mcguffin_t;

	//! Busy mcguffin.

	busy_mcguffin_t busy_mcguffin;

	//! Whether we're busy (ignore keyboard or button press events)

	bool is_busy();

	//! An element's button_press_event() wishes to grab the pointer.

	void grab(IN_THREAD_ONLY, const ref<elementObj::implObj> &e);
 private:

	//! Forward key event to the X Input Method server.

	//! If XIM not available, call handle_key_event().
	void forward_key_event(IN_THREAD_ONLY,
			       const xcb_key_release_event_t *event,
			       uint16_t sequencehi,
			       bool keypress);

	//! Process a key press or release event.
	void handle_key_event(IN_THREAD_ONLY,
			      const xcb_key_release_event_t *event,
			      bool keypress) override;

	//! Process a button press or release event.

	//! Finds the display element under the pointer and invokes its
	//! process_button_event(). If the button event does not get processed
	//! and it is a button #1 press, the keyboard input focus gets removed
	//! from the element that currently has the keyboard input focus.

	void do_button_event(IN_THREAD_ONLY,
			     const xcb_button_release_event_t *event,
			     bool buttonpress,
			     bool was_grabbed);

	//! Use grab_locked() for was_grabbed in the overloaded method.

	void do_button_event(IN_THREAD_ONLY,
			     const xcb_button_release_event_t *event,
			     bool buttonpress);

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
	void set_window_title(const std::experimental::string_view &s);

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

	current_fontcollection create_theme_font(const std::experimental::string_view &font);

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

	//! Look up focusables that installed a shortcut.

	shortcut_lookup_t shortcut_lookup_thread_only;

 public:
	THREAD_DATA_ONLY(focusable_fields);
	THREAD_DATA_ONLY(most_recent_keyboard_focus);
	THREAD_DATA_ONLY(shortcut_lookup);

	//! Move the current keyboard focus to another element.
	void set_keyboard_focus_to(IN_THREAD_ONLY, const focusable_impl &f);

	//! No element has keyboard focus any more.
	void unset_keyboard_focus(IN_THREAD_ONLY);

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
			   const std::experimental::u32string_view &)
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

};

LIBCXXW_NAMESPACE_END

#endif
