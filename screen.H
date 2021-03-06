/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef screen_h
#define screen_h

#include "x/w/screen.H"
#include "x/w/screen_depthinfo.H"
#include "x/w/picturefwd.H"
#include "x/w/pictformatfwd.H"
#include "x/w/scratch_bufferfwd.H"
#include "x/w/border_arg.H"
#include "x/w/input_field_configfwd.H"
#include "recycled_pixmapsfwd.H"
#include "x/w/rgb.H"
#include "connection.H"
#include "x/w/connection_threadfwd.H"
#include "x/w/defaultthemefwd.H"
#include "x/w/impl/background_colorfwd.H"
#include "screen_picturecachefwd.H"
#include "screen_fontcachesfwd.H"
#include "ellipsiscachefwd.H"
#include "x/w/impl/border_implfwd.H"
#include "x/w/impl/iconfwd.H"
#include "border_cachefwd.H"
#include "xid_t.H"
#include "fonts/fontconfigfwd.H"
#include "fonts/freetypefwd.H"
#include "icon_cachefwd.H"
#include "cursor_pointer_cachefwd.H"
#include <x/logger.H>
#include <x/vector.H>
#include <xcb/xcb.h>
#include <cmath>
#include <string>
#include <unordered_set>

LIBCXXW_NAMESPACE_START

class render;

//! Find or create a border.

current_border_impl get_cached_border(const screen &s,
				      const const_pictformat &pf,
				      const border_arg &arg)
		LIBCXX_HIDDEN;

//! Create a new icon
icon create_new_icon(const screen &screenref,
		     const const_pictformat &drawable_pictformat,
		     const image_color &args) LIBCXX_HIDDEN;


//! Create a cached gradient background color
background_color create_new_gradient_background_color
(const screen &s,
 const background_color &base_color,
 const const_picture &p) LIBCXX_HIDDEN;

//! Create a background color from the current theme.

//! This returns a background_color object. Its get_current_color()
//! method returns the background color from the current theme. If
//! the theme changes, the returned color will be from the new theme.
//!
//! The returned object holds a reference to this screen object.

background_color create_new_background_color(const screen &s,
					     const const_pictformat &pf,
					     const color_arg &color_name);

//! Return a non-theme background_color object.

background_color create_new_background_color(const screen &s,
					     const const_pictformat &pf,
					     const const_picture &pic);

//! xcb_screen_t owner.

class LIBCXX_HIDDEN screenObj::implObj
	: virtual public obj {

	//! Even though we use RENDER, CreateWindow() needs a colormap...

	class toplevelwindow_colormapObj
		: virtual public obj,
		  public xid_t<xcb_colormap_t> {
	public:
		toplevelwindow_colormapObj(const connection_thread &thread,
					   xcb_window_t window,
					   xcb_visualid_t visual);

		~toplevelwindow_colormapObj();
	};

 public:

	LOG_CLASS_SCOPE;

	//////////////////////////////////////////////////////////////////
	//
	// Class members

	//! This screen
	const xcb_screen_t * const xcb_screen;

	//! This screen's number
	const size_t screen_number;

	//! The connection thread
	const connection_thread thread;

	//! Use this visual for top level windows.
	const screen::base::visual_t toplevelwindow_visual;

	//! Use this pictformat for top level windows
	const const_pictformat toplevelwindow_pictformat;

	//! Temporary colormap for top level windows
	const ref<toplevelwindow_colormapObj> toplevelwindow_colormap;

	//! Same screen_depths as the public object.
	const vector<const_ref<depthObj>> screen_depths;

	//! Supported protocols, the atoms ewmh sets in _NET_SUPPORTED
	const std::unordered_set<std::string> supported;

	//! The current theme for this screen.
	current_theme_t current_theme;

	mpobj<bidi_format> default_bidi_format;

	//! The fontconfig library

	const fontconfig::config fc;

	//! The freetype library

	const freetype ft;

	//! A cache of solid color picture objects

	//! Used by create_solid_color_picture().
	const screen_picturecache picturecache;

	//! A cache of recycled pixtures, pixmaps, and background colors

	const recycled_pixmaps recycled_pixmaps_cache;

	//! The custom border cache.

	//! Used by get_cached_border().

	const border_cache screen_border_cache;

	//! Font caches

	//! Use by create_font().

	const screen_fontcaches fontcaches;

	//! Label ellipsis cache.

	//! Cache for the ellipsis string that's shown when the label is
	//! too long to fit within its alloted width.

	const ellipsiscache ellipsiscaches;

	//! Loaded icons for this screen.

	const icon_cache iconcaches;

	//! Loaded pointers for this screen.
	const cursor_pointer_cache cursor_pointercaches;

	///////////////////////////////////////////////////////////////////
	//
	// Construction

	//! Create the screen_depths. Called from the constructor.

	static vector<const_ref<depthObj>>
		create_screen_depths(const xcb_screen_t *,
				     const render &,
				     size_t);

	//! Constructor
	implObj(const xcb_screen_t *xcb_screen,
		size_t screen_number,
		const render &render_info,
		const vector<const_ref<depthObj>> &screen_depths,
		const std::unordered_set<std::string> &supported,
		const defaulttheme &current_theme,
		const screen::base::visual_t &toplevelwindow_visual,
		const const_pictformat &toplevelwindow_pictformat,
		const connection_thread &thread);

	//! Destructor
	~implObj();

	//! Invoked from the connection thread to update current_theme

	void update_current_theme(ONLY IN_THREAD,
				  const const_defaulttheme &new_theme);

	////////////////////////////////////////////////////////////////
	//
	// Screen dimension for informational purposes only.

	//! Get screen dimensions

	dim_t width_in_pixels() const
	{
		return dim_t(xcb_screen->width_in_pixels);
	}

	//! Get screen dimensions

	dim_t height_in_pixels() const
	{
		return dim_t(xcb_screen->height_in_pixels);
	}

	//! Get screen dimensions

	dim_t width_in_millimeters() const
	{
		return dim_t(xcb_screen->width_in_millimeters);
	}

	//! Get screen dimensions

	dim_t height_in_millimeters() const
	{
		return dim_t(xcb_screen->height_in_millimeters);
	}

	// Return root window's depth.

	depth_t root_depth() const;

	//! Return root window's visual

	screen::base::visual_t root_visual() const;

	//! Return root window's visual.

	static screen::base::visual_t
		root_visual(const xcb_screen_t *xcb_screen,
			    const vector<const_ref<depthObj>> &screen_depths);

	/////////////////////////////////////////////////////////////////////
	//
	// Lock the current theme, and obtain the theme-specific data.

	//! Retrieve theme color
	theme_color_t get_theme_color(const std::string &theme_color);

	//! Find or create a theme border

	//////////////////////////////////////////////////////////////////////
	//
	// Colors

	//! Create a cached solid color picture;

	const_picture create_solid_color_picture(const rgb &color);

	//! Create a cached linear gradient picture
	const_picture create_linear_gradient_picture(const linear_gradient &g,
						     coord_t offset_x,
						     coord_t offset_y,
						     dim_t w,
						     dim_t h,
						     render_repeat repeat);

	//! Create a cached radial gradient picture
	const_picture create_radial_gradient_picture(const radial_gradient &g,
						     coord_t offset_x,
						     coord_t offset_y,
						     dim_t w,
						     dim_t h,
						     render_repeat repeat);

	friend background_color create_new_gradient_background_color
		(const screen &s,
		 const background_color &base_color,
		 const const_picture &p);

	friend background_color
		create_new_background_color(const screen &s,
					    const const_pictformat &pf,
					    const color_arg &color_name);

	friend background_color
		create_new_background_color(const screen &s,
					    const const_pictformat &pf,
					    const const_picture &pic);

	friend current_border_impl get_cached_border(const screen &s,
						     const const_pictformat &pf,
						     const border_arg &arg);

	/////////////////////////////////////////////////////////////////////
	//
	// Temporary pixmaps and other objects.

	//! Create a temporary buffer for building images.

	//! The returned \ref scratch_buffer "scratch_buffer" object may
	//! be an existing object (it comes from a weak map pool). If a new
	//! shared picture needs to be created, its size is specified by
	//! \c initial_width and \c initial_height.

	scratch_buffer create_scratch_buffer(const screen &public_object,
					     const std::string &identifier,
					     const const_pictformat &pf);
};

LIBCXXW_NAMESPACE_END

#endif
