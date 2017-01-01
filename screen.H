/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef screen_h
#define screen_h

#include "x/w/screen.H"
#include "x/w/screen_depthinfo.H"
#include "x/w/picturefwd.H"
#include "x/w/rgbfwd.H"
#include "connection.H"
#include "connection_threadfwd.H"
#include "xid_t.H"
#include <x/logger.H>
#include <x/vector.H>
#include <x/weakmultimapfwd.H>
#include <xcb/xcb.h>

#include <cmath>

LIBCXXW_NAMESPACE_START

class render;

// A cache of solid color pictures for this screen.

class LIBCXX_HIDDEN screen_solidcolorpictures {

 public:
	typedef x::weakmultimap<rgb, pictureObj> map_t;

	map_t map;

	screen_solidcolorpictures();

	~screen_solidcolorpictures() noexcept;
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

		~toplevelwindow_colormapObj() noexcept;
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

	// Create the screen_depths. Called from the constructor.

	static vector<const_ref<depthObj>>
		create_screen_depths(const xcb_screen_t *,
				     const render &,
				     size_t);

	implObj(const xcb_screen_t *xcb_screen,
		size_t screen_number,
		const render &render_info,
		const vector<const_ref<depthObj>> &screen_depths,
		const screen::base::visual_t &toplevelwindow_visual,
		const const_pictformat &toplevelwindow_pictformat,
		const connection_thread &thread);
	~implObj() noexcept;

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

	// Compute # of pixels from millimeters

	dim_t compute_width(double millimeters) const
	{
		return dim_t(std::round(theme_scale() * millimeters *
					xcb_screen->width_in_pixels /
					xcb_screen->width_in_millimeters));
	}

	dim_t compute_height(double millimeters) const
	{
		return dim_t(std::round(theme_scale() * millimeters *
					xcb_screen->height_in_pixels /
					xcb_screen->height_in_millimeters));
	}
};

LIBCXXW_NAMESPACE_END

#endif
