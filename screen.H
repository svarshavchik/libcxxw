#ifndef screen_h
#define screen_h

#include "x/w/screen.H"
#include "x/w/screen_depthinfo.H"
#include "x/w/picturefwd.H"
#include "x/w/rgbfwd.H"
#include "connection.H"
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

 public:

	LOG_CLASS_SCOPE;

	const xcb_screen_t * const xcb_screen;
	const size_t screen_number;
	const ref<connectionObj::implObj::infoObj> info;

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
		const ref<connectionObj::implObj::infoObj> &info);
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
