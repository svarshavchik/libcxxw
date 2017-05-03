/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef defaultthemeobj_H
#define defaultthemeobj_H

#include <x/xml/doc.H>
#include <x/functional.H>

#include "x/w/drawablefwd.H"
#include "x/w/rgb.H"
#include "x/w/font.H"
#include "x/w/picturefwd.H"
#include "x/w/screenfwd.H"
#include "x/w/screenobj.H"
#include "x/w/pictformatfwd.H"
#include "connection_threadfwd.H"
#include "defaultthemefwd.H"
#include "border_implfwd.H"

#include <string>
#include <unordered_map>
#include <xcb/xproto.h>
#include <experimental/string_view>

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

	//! Borders
	std::unordered_map<std::string, const_border_impl> borders;

	//! Fonts
	std::unordered_map<std::string, font> fonts;

	//! Parse dimensions from the configuration file.
	void load_dims(const xml::doc &config);

	//! Parse colors from the configuration file.
	void load_colors(const xml::doc &config);

	//! Parse gradients from the configuration file.
	void load_color_gradients(const xml::doc &config);

	//! Parse boders from the configuration file.
	void load_borders(const xml::doc &config,
			  const ref<screenObj::implObj> &screen);

	void load_fonts(const xml::doc &config);

 public:

	//! Load font definitions.

	//! This is also used by the SXG file parser.
	//! The functor does not get invoked if the <font> specifies a
	//! "from" that has not been parsed yet.

	template<typename install_functor, typename lookup_functor>
		void load_fonts(const xml::doc::base::readlock &lock,
				const xml::doc::base::xpath &xpath,
				install_functor &&install,
				lookup_functor &&lookup)
	{
		do_load_fonts(lock,
			      xpath,
			      make_function<void (const std::string &,
						  const font &)>
			      (std::forward<install_functor>(install)),
			      make_function<bool (const std::string &, font &)>
			      (std::forward<lookup_functor>(lookup)));
	}

	//! Load font definitions.

	void do_load_fonts(const xml::doc::base::readlock &lock,
			   const xml::doc::base::xpath &xpath,
			   const function<void (const std::string &,
						const font &)> &install,
			   const function<bool (const std::string &,font &)>
			   &lookup);

	//! Retrieve theme dimension
	dim_t get_theme_dim_t(const std::experimental::string_view &id,
			      dim_t default_value);

	//! Retrieve theme color
	rgb get_theme_color(const std::experimental::string_view &id,
			    const rgb &default_value);

	//! Retrieve theme gradient
	rgb::gradient_t get_theme_color_gradient(const std::experimental::string_view &id,
						 const rgb::gradient_t
						 &default_value);

	//! Retrieve a theme border.
	const_border_impl get_theme_border(const std::experimental::string_view
					   &id,
					   const border_info &default_value);
	//! Retrieve a theme border.
	const_border_impl get_theme_border(const std::experimental::string_view
					   &id,
					   const const_border_impl
					   &default_value);
	//! Retrieve a theme font.
	font get_theme_font(const std::string &id,
			    const font &default_value);

	/////////////////////////////////////////////////////////////////////
	//
	// Construction and initialization

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

		//! Theme directory
		std::string themedir;

		//! Determined theme scale
		double themescale;

		//! The loaded theme configuration file, null if parsing error.
		xml::docptr theme_configfile;
	};


	//! Construct a theme for the given screen.

	//! This construct the object, but does not fully initialize it.
	//! After constructing the theme object, load() must be invoked with
	//! the same theme_config parameter.
	defaultthemeObj(const xcb_screen_t *screen,
			const config &theme_config);

	//! Actually load the theme into the newly-constructed object.

	//! Finish the construction. Must get the same theme_config that
	//! the constructor got, plus a reference to the theme's screen.

	void load(const config &theme_config,
		  const ref<screenObj::implObj> &screen);

	~defaultthemeObj();
};

LIBCXXW_NAMESPACE_END
#endif
