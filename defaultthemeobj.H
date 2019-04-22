/*
** Copyright 2017-2019 Double Precision, Inc.
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
#include "x/w/text_paramfwd.H"
#include "x/w/dim_arg.H"
#include "x/w/connectionfwd.H"
#include "x/w/connection_threadfwd.H"
#include "x/w/defaultthemefwd.H"
#include "x/w/impl/themedimfwd.H"
#include "x/w/uigeneratorsobj.H"
#include "theme_parser_lockfwd.H"

#include <string>
#include <unordered_map>
#include <xcb/xproto.h>
#include <string_view>

LIBCXXW_NAMESPACE_START

class connection_implObj;

void load_cxxwtheme_property(const screen &screen0,
			     const std::string &theme_name,
			     int theme_scale,
			     const enabled_theme_options_t
			     &enabled_theme_options) LIBCXX_PUBLIC;

void load_cxxwtheme_property(const xcb_screen_t *screen_0,
			     const connection_thread &thread,
			     const std::string &theme_name,
			     int theme_scale,
			     const enabled_theme_options_t
			     &enabled_theme_options) LIBCXX_HIDDEN;

//////////////////////////////////////////////////////////////////////////////
//
// The window theme

class LIBCXX_HIDDEN defaultthemeObj : public uigeneratorsObj {

public:
	//! Name of this theme
	const std::string themename;

	//! Scaling factor
	const double themescale;

	//! All available options for this theme.

	const std::vector<theme_option> available_theme_options;

	//! Which options are enabled.
	const enabled_theme_options_t enabled_theme_options;

	//! See if two theme objects are the same

	//! cxxwtheme updates the screen's theme justbefore it terminates.
	//! This sends back to us the notification for the property change,
	//! which often happens before we start the process of closing the
	//! main window. The program termination gets needlessly delayed while
	//! the repercurssions of the updated theme are digested. The newly
	//! set theme is the same one that's already set in its window, so
	//! this provides the means of detecting it.

	bool is_different_theme(const const_defaulttheme &t) const;

	const std::string themedir;

	//! Horizontal distance of 1 millimeter, in pixels

	const dim_t h1mm;

	//! Vertical distance of 1 millimeter, in pixels

	const dim_t v1mm;

 private:

	//! My screen
	const xcb_screen_t *screen;

	//! Fonts
	std::unordered_map<std::string, font> fonts;

	//! Parse fonts from the configuration file
	void load_fonts(const theme_parser_lock &root_lock);

 public:
	//! Load font definitions.

	//! This is also used by the SXG file parser.
	//! The functor does not get invoked if the <font> specifies a
	//! "from" that has not been parsed yet.

	template<typename install_functor, typename lookup_functor>
		void load_fonts(const theme_parser_lock &lock,
				const xml::doc::base::xpath &xpath,
				install_functor &&install,
				lookup_functor &&lookup) const
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

	void do_load_fonts(const theme_parser_lock &lock,
			   const xml::doc::base::xpath &xpath,
			   const function<void (const std::string &,
						const font &)> &install,
			   const function<bool (const std::string &,font &)>
			   &lookup) const;

	//! Retrieve theme dimension

	//! The given dimension is specified in the theme. The dimension
	//! name may also be a numerical value, given as a literal string,
	//! which is interpreted as millimeters; in which case themedimaxis
	//! specifies whether this is a horizontal or a vertical dimension.

	dim_t get_theme_dim_t(const dim_arg &id, themedimaxis wh) const;

	//! Compute # of pixels from millimeters, for width

	dim_t compute_width(double millimeters) const;

	//! Compute # of pixels from millimeters, for height

	dim_t compute_height(double millimeters) const;

	//! Retrieve theme color
	theme_color_t get_theme_color(const std::string_view &theme_color)
		const;

	//! Retrieve a theme border.
	const border_infomm &get_theme_border(const std::string &id)
		const;

	//! Retrieve a theme font.
	font get_theme_font(const std::string &id) const;

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

		//! Enabled options
		enabled_theme_options_t enabled_theme_options;

		//! The loaded theme configuration file.
		xml::doc theme_configfile;
	};


	//! Construct a theme for the given screen.

	//! This construct the object, but does not fully initialize it.
	//! After constructing the theme object, load() must be invoked with
	//! the same theme_config parameter.
	defaultthemeObj(const xcb_screen_t *screen,
			const config &theme_config);

	~defaultthemeObj();

	//! Default cut/paste selection
	const char *default_cut_paste_selection() const;
};

LIBCXXW_NAMESPACE_END
#endif
