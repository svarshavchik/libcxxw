/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef screen_h
#define screen_h

#include "x/w/screen.H"
#include "x/w/screen_depthinfo.H"
#include "x/w/picturefwd.H"
#include "x/w/rgb.H"
#include "connection.H"
#include "connection_threadfwd.H"
#include "defaultthemefwd.H"
#include "xid_t.H"
#include <x/logger.H>
#include <x/vector.H>
#include <x/mpobj.H>
#include <x/weakunordered_multimapfwd.H>
#include <xcb/xcb.h>

#include <cmath>

LIBCXXW_NAMESPACE_START

class render;

// A cache of solid color pictures for this screen.

class LIBCXX_HIDDEN screen_solidcolorpictures {

 public:
	typedef x::weakunordered_multimap<rgb, pictureObj> map_t;

	map_t map;

	screen_solidcolorpictures();

	~screen_solidcolorpictures();
};

//! xcb_screen_t owner.

class LIBCXX_HIDDEN screenObj::implObj
	: virtual public obj,
	  public screen_solidcolorpictures {

	// Even though we use RENDER, CreateWindow() needs a colormap...

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

	const xcb_screen_t * const xcb_screen;
	const size_t screen_number;
	const connection_thread thread;

	// Use this visual, pictformat, and colormap for top level windows
	const screen::base::visual_t toplevelwindow_visual;
	const const_pictformat toplevelwindow_pictformat;
	const ref<toplevelwindow_colormapObj> toplevelwindow_colormap;

	// Same screen_depths as the public object.
	const vector<const_ref<depthObj>> screen_depths;

	// Current theme, mutex protected.
	//
	// The current theme must be consulted when constructing new
	// display elements, and it is also accessed by the connection thread
	// as well.

	typedef mpobj<defaulttheme> current_theme_t;

	current_theme_t current_theme;

	// Create the screen_depths. Called from the constructor.

	static vector<const_ref<depthObj>>
		create_screen_depths(const xcb_screen_t *,
				     const render &,
				     size_t);

	implObj(const xcb_screen_t *xcb_screen,
		size_t screen_number,
		const render &render_info,
		const vector<const_ref<depthObj>> &screen_depths,
		const defaulttheme &current_theme,
		const screen::base::visual_t &toplevelwindow_visual,
		const const_pictformat &toplevelwindow_pictformat,
		const connection_thread &thread);
	~implObj();

	// Get screen dimensions

	dim_t width_in_pixels() const
	{
		return dim_t(xcb_screen->width_in_pixels);
	}

	dim_t height_in_pixels() const
	{
		return dim_t(xcb_screen->height_in_pixels);
	}

	dim_t width_in_millimeters() const
	{
		return dim_t(xcb_screen->width_in_millimeters);
	}

	dim_t height_in_millimeters() const
	{
		return dim_t(xcb_screen->height_in_millimeters);
	}

	double theme_scale() const
	{
		return 1;
	}

	// Return root window's depth.

	depth_t root_depth() const;

	// Return root window's visual

	screen::base::visual_t root_visual() const;

	static screen::base::visual_t
		root_visual(const xcb_screen_t *xcb_screen,
			    const vector<const_ref<depthObj>> &screen_depths);

	// Lock the current theme, and obtain the theme-specific data.

	//! Retrieve theme dimension
	dim_t get_theme_dim_t(const std::string &id,
			      dim_t default_value);

	//! Retrieve theme color
	rgb get_theme_color(const std::string &id,
				   const rgb &default_value);

	//! Retrieve theme gradient
	rgb::gradient_t get_theme_color_gradient(const std::string &id,
						 const rgb::gradient_t
						 &default_value);

	// Compute # of pixels from millimeters, for width or height.

	// Given number must be possible. It is truncated at the largest
	// possible dim_t value.

	dim_t compute_width(double millimeters) const
	{
		auto scaled=
			std::round(theme_scale() * millimeters *
				   xcb_screen->width_in_pixels /
				   xcb_screen->width_in_millimeters);

		if (scaled > dim_t::infinite()-1)
			scaled=dim_t::infinite()-1;

		return dim_t::value_type(scaled);
	}

	dim_t compute_height(double millimeters) const
	{
		auto scaled=
			std::round(theme_scale() * millimeters *
				   xcb_screen->height_in_pixels /
				   xcb_screen->height_in_millimeters);

		if (scaled > dim_t::infinite()-1)
			scaled=dim_t::infinite()-1;

		return dim_t::value_type(scaled);
	}
};

LIBCXXW_NAMESPACE_END

#endif
