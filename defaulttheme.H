/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef defaulttheme_h
#define defaulttheme_h

#include <x/ref.H>
#include "x/w/defaultthemefwd.H"
#include "defaultthemeobj.H"

LIBCXXW_NAMESPACE_START

extern const char * const rgb_channels[4] LIBCXX_HIDDEN;

extern rgb_component_t rgb::* const rgb_fields[4] LIBCXX_HIDDEN;

//! Base class for loaded themes

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::defaulttheme::base

class defaultthemeBase : public ptrref_base {

public:

	//! Convenience typedef
	typedef defaultthemeObj::config config;

	//! Load the theme configuration.

	//! Passing in the CXXWTHEME property from screen 0's root window,
	//! Possibly empty if unset.
	//!
	//! The returned config object gets used to construct a defaulttheme.
	static config get_config(const std::string &property) LIBCXX_HIDDEN;

	//! Load a custom theme configuration.

	//! The returned config object gets used to construct a defaulttheme.
	static config get_config(const std::string &themename,
				 double themescale,
				 const enabled_theme_options_t
				 &enabled_theme_options) LIBCXX_HIDDEN;

	//! Read the CXXWTHEME propert from screen  0's root window.

	static std::string cxxwtheme_property(const xcb_screen_t *screen_0,
					      const connection_thread &thread)
		LIBCXX_HIDDEN;
};


LIBCXXW_NAMESPACE_END

#endif
