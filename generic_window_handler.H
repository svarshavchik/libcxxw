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
#include "background_colorfwd.H"
#include "connection_threadfwd.H"
#include "container.H"
#include "x/w/pictformatfwd.H"
#include "x/w/types.H"
#include "fonts/current_fontcollectionfwd.H"
#include "x/w/fontfwd.H"
#include <experimental/string_view>

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

// The container
	  public containerObj::implObj,

// The display element.
	  public elementObj::implObj {

	//! Current event mask.
	xcb_event_mask_t current_events_thread_only;

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

	//! Window's background color

	background_color current_background_color_thread_only;

	//! The first time the window is mapped, it's resized to its preferred size, first.

	bool preferred_dimensions_set_thread_only=false;

	//! Preferred width
	dim_t preferred_width_thread_only;

	//! Preferred height
	dim_t preferred_height_thread_only;

 public:

	//! What I want for my initial event mask.

	static constexpr xcb_event_mask_t initial_event_mask()
	{
		return (xcb_event_mask_t)(XCB_EVENT_MASK_EXPOSURE |
					  XCB_EVENT_MASK_VISIBILITY_CHANGE |
					  XCB_EVENT_MASK_STRUCTURE_NOTIFY);
	}

	//! What my constructor needs

	struct constructor_params {

		//! Parameters for window_handler's constructor.

		window_handlerObj::constructor_params window_handler_params;

		//! The window's pictformat.

		const_pictformat drawable_pictformat;
	};

	handlerObj(IN_THREAD_ONLY,
		   const constructor_params &params);

	//! Destructor
	~handlerObj();

	THREAD_DATA_ONLY(current_events);
	THREAD_DATA_ONLY(current_background_color);
	THREAD_DATA_ONLY(preferred_dimensions_set);
	THREAD_DATA_ONLY(preferred_width);
	THREAD_DATA_ONLY(preferred_height);

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

	void remove_background_color(IN_THREAD_ONLY) override;

	//! Use its own background color.

	void set_background_color(IN_THREAD_ONLY,
				  const background_color &) override;

	void current_position_updated(IN_THREAD_ONLY) override;

#include "container_element_overrides_decl.H"

	////////////////////////////////////////////////////////////////////
	//
	// Inherited from window_handler

	//! Implement the exposure event.

	void exposure_event(IN_THREAD_ONLY,
			    rectangle_set &areas) override;

	//! Theme update notification.
	void theme_updated_event(IN_THREAD_ONLY) override;

	//! Do not redraw after the window's visibility is updated.

	//! Instead, we redraw in response to exposure events.

	void draw_child_elements_after_visibility_updated(IN_THREAD_ONLY,
							  bool flag) override;

	//! Setting the inherited_visibility flag.

	//! This gets translated to mapping/unmapping the window.

	void set_inherited_visibility(IN_THREAD_ONLY,
				      inherited_visibility_info &info) override;

	//! Set window size to preferred size before its visible for 1st time.

	void request_visibility(IN_THREAD_ONLY, bool flag) override;

	//! Whether this window is mapped.

	bool is_mapped(IN_THREAD_ONLY) const
	{
		return data(IN_THREAD).inherited_visibility;
	}

	//! Update the displayed window's element's current_position

	void configure_notify(IN_THREAD_ONLY,
			      const rectangle &) override;

	//! Return the window manager's hints.

	bool get_frame_extents(dim_t &left,
			       dim_t &right,
			       dim_t &top,
			       dim_t &bottom)
		const;

	//! Inherited from horizvertObj

	void horizvert_updated(IN_THREAD_ONLY) override;

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

};

LIBCXXW_NAMESPACE_END

#endif
