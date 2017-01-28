#ifndef defaultthemeobj_H
#define defaultthemeobj_H

#include <x/xml/doc.H>
#include "x/w/drawablefwd.H"
#include "x/w/rgb.H"
#include "x/w/picturefwd.H"
#include "x/w/screenfwd.H"
#include "x/w/screenobj.H"
#include "x/w/pictformatfwd.H"
#include "connection_threadfwd.H"
#include "defaultthemefwd.H"

#include <string>
#include <unordered_map>
#include <xcb/xproto.h>

LIBCXXW_NAMESPACE_START

class connection_implObj;

void load_cxxwtheme_property(const screen &screen0,
			     const std::string &theme_name,
			     int theme_scale) LIBCXX_PUBLIC;

//////////////////////////////////////////////////////////////////////////////
//
// The window theme

class LIBCXX_HIDDEN defaultthemeObj : virtual public obj {

public:
	//! Name of this theme
	const std::string themename;

	//! Scaling factor
	const double themescale;

	//! Horizontal distance of 1 millimeter, in pixels

	const dim_t h1mm;

	//! Vertical distance of 1 millimeter, in pixels

	const dim_t v1mm;

 private:

	//! Parsed dimensions
	std::unordered_map<std::string, dim_t> dims;

	//! Parsed colors
	std::unordered_map<std::string, rgb> colors;

	//! Parsed gradients
	std::unordered_map<std::string, rgb::gradient_t> color_gradients;

	//! Parse dimensions from the configuration file.
	void load_dims(const xml::doc &config);

	//! Parse colors from the configuration file.
	void load_colors(const xml::doc &config);

	//! Parse gradients from the configuration file.
	void load_color_gradients(const xml::doc &config);

 public:

	//! Retrieve theme dimension
	dim_t get_theme_dim_t(const std::string &id,
			      dim_t default_value);

	//! Retrieve theme color
	const rgb &get_theme_color(const std::string &id,
				   const rgb &default_value);

	//! Retrieve theme gradient
	const rgb::gradient_t &get_theme_color_gradient(const std::string &id,
							const rgb::gradient_t
							&default_value);

	//! Structure returned by \c INSERT_LIBX_NAMESPACE::w::defaulttheme::base::get_config()

	struct config {

		//! Screen #0
		const xcb_screen_t *screen_0;

		//! The connection thread.
		connection_thread thread;

		//! CXXWTHEME property on screen_0's root window, if set.
		std::string cxxwtheme_property;

		//! Determined theme name
		std::string themename;

		//! Determined theme scale
		double themescale;

		//! The loaded theme configuration file, null if parsing error.
		xml::docptr theme_configfile;
	};


	//! Constructor a theme for the given screen.

	defaultthemeObj(const xcb_screen_t *screen,
			const config &theme_config);

	~defaultthemeObj();
};

LIBCXXW_NAMESPACE_END
#endif
