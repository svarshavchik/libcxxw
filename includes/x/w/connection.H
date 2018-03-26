/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_connection_h
#define x_w_connection_h

#include <x/w/connectionfwd.H>
#include <x/w/connectionobj.H>
#include <x/ref.H>
#include <x/functionalrefptr.H>
#include <string>
#include <vector>

LIBCXXW_NAMESPACE_START

#include <x/w/deps/screen_connection.H>

//! Screen themes have options.

struct theme_option {
	std::string	label;		//!< Internal label
	std::u32string	description;	//!< Description
	bool		selected;	//!< If this theme option is selected
};

//! Base class for \ref connection "display server connections".

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::connection::base

class connectionBase : public ptrref_base {

public:
	//! Values returned from available_themes()

	struct available_theme {

		//! Identifier for the given theme
		std::string identifier;

		//! Its name or description, in UTF-8.
		std::string description;

		//! All available options for this theme.

		//! Each option's \c selected flag is false.
		std::vector<theme_option> available_options;
	};

	//! Enumerate available themes.

	static std::vector<available_theme> available_themes();
};

LIBCXXW_NAMESPACE_END

#endif
