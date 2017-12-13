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
#include "x/w/gridlayoutmanagerfwd.H"
#include "x/w/gridfactoryfwd.H"
#include "x/w/dim_arg.H"
#include "connection_threadfwd.H"
#include "defaultthemefwd.H"
#include "border_implfwd.H"
#include "gridtemplatefwd.H"

#include <string>
#include <unordered_map>
#include <xcb/xproto.h>
#include <string_view>

LIBCXXW_NAMESPACE_START

class connection_implObj;

void load_cxxwtheme_property(const screen &screen0,
			     const std::string &theme_name,
			     int theme_scale) LIBCXX_PUBLIC;

void load_cxxwtheme_property(const xcb_screen_t *screen_0,
			     const connection_thread &thread,
			     const std::string &theme_name,
			     int theme_scale) LIBCXX_HIDDEN;

//////////////////////////////////////////////////////////////////////////////
//
// The window theme

class LIBCXX_HIDDEN defaultthemeObj : virtual public obj {

public:
	//! Name of this theme
	const std::string themename;

	//! Scaling factor
	const double themescale;

	//! The directory this theme was loaded from
	const std::string themedir;

	//! Horizontal distance of 1 millimeter, in pixels

	const dim_t h1mm;

	//! Vertical distance of 1 millimeter, in pixels

	const dim_t v1mm;

 private:

	//! My screen
	const xcb_screen_t *screen;

	//! Parsed dimensions
	std::unordered_map<std::string, dim_t> dims;

	//! Parsed colors
	std::unordered_map<std::string, rgb> colors;

	//! Parsed gradients
	std::unordered_map<std::string, rgb_gradient> color_gradients;

	//! Borders
	std::unordered_map<std::string, const_border_impl> borders;

	//! Fonts
	std::unordered_map<std::string, font> fonts;

	//! Type-erased layout generator.

	typedef std::function<void (const gridlayoutmanager &,
				    const gridtemplate * &,
				    const defaulttheme &)> layout_generator_t;

	//! Layouts

	std::unordered_map<std::string, std::vector<layout_generator_t>
			   > layouts;

	//! Type-erased factory generator.

	typedef std::function<void (const gridfactory &,
				    const gridtemplate * &,
				    const defaulttheme &)> factory_generator_t;

	//! Factories
	std::unordered_map<std::string,
			   std::vector<factory_generator_t>> factories;

	//! Parse dimensions from the configuration file.
	void load_dims(const xml::doc &config);

	//! Parse colors from the configuration file.
	void load_colors(const xml::doc &config);

	//! Parse gradients from the configuration file.
	void load_color_gradients(const xml::doc &config);

	//! Parse boders from the configuration file.
	void load_borders(const xml::doc &config,
			  const ref<screenObj::implObj> &screen);

	//! Parse fonts from the configuration file
	void load_fonts(const xml::doc &config);

	//! Parse layouts from the configuration file
	void load_layouts(const xml::doc &config);

	//! Parse elements from the configuration file
	void load_factories(const xml::doc &config);

	//! Stylesheet-generated parsing code.
	static layout_generator_t
		gridlayout_parser(const xml::doc::base::readlock &lock);

	//! Stylesheet-generated parsing code.
	static void
		gridlayout_parseconfig(const xml::doc::base::readlock &lock,
				       std::vector<layout_generator_t> &);

	//! Stylesheet-generated parsing code.
	static factory_generator_t
		gridfactory_parser(const xml::doc::base::readlock &lock);

	//! Stylesheet-generated parsing code.
	static void
		gridfactory_parseconfig(const xml::doc::base::readlock &lock,
					std::vector<factory_generator_t> &);

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

	//! The given dimension is specified in the theme. The dimension
	//! name may also be a numerical value, given as a literal string,
	//! which is interpreted as millimeters.
	dim_t get_theme_width_dim_t(const dim_arg &id);

	//! Retrieve theme dimension

	//! The given dimension is specified in the theme. The dimension
	//! name may also be a numerical value, given as a literal string,
	//! which is interpreted as millimeters.
	dim_t get_theme_height_dim_t(const dim_arg &id);

	//! Compute # of pixels from millimeters, for width

	dim_t compute_width(double millimeters);

	//! Compute # of pixels from millimeters, for height

	dim_t compute_height(double millimeters);

	//! Retrieve theme color
	rgb get_theme_color(const color_arg &color) const;

	//! Retrieve theme gradient
	rgb_gradient get_theme_color_gradient(const rgb_gradient_arg &arg)
		const;

	//! Retrieve a theme border.
	const_border_impl get_theme_border(const std::string_view
					   &id);

	//! Retrieve a theme font.
	font get_theme_font(const std::string &id);

	/////////////////////////////////////////////////////////////////////
	//
	// Construction and initialization

	//! Structure returned by \c INSERT_LIBX_NAMESPACE::w::defaulttheme::base::get_config()

	struct config {
		//! Determined theme name
		std::string themename;

		//! Theme directory
		std::string themedir;

		//! Determined theme scale
		double themescale;

		//! The loaded theme configuration file.
		xml::doc theme_configfile;
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

	void load(const xml::doc &config,
		  const ref<screenObj::implObj> &screen);

	~defaultthemeObj();

 private:

	//! Add a new row to the grid layout manager.
	void layout_append_row(const gridlayoutmanager &,
			       const gridtemplate *,
			       const std::string &);

	//! Insert another template

	void layout_insert(const gridfactory &,
			   const gridtemplate *,
			   const std::string &,
			   const std::string &);
 public:
	//! Insert a theme template
	void layout_insert(const gridlayoutmanager &glm,
			   const gridtemplate *elements,
			   const std::string &name);
};

LIBCXXW_NAMESPACE_END
#endif
