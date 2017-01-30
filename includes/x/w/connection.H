/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_connection_h
#define x_w_connection_h

#include <x/w/connectionfwd.H>
#include <x/w/connectionobj.H>
#include <x/ref.H>
#include <string>
#include <vector>

LIBCXXW_NAMESPACE_START

#include <x/w/deps/screen_connection.H>

//! Base class for \ref connection "display server connections".

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::connection::base

class connectionBase : public ptrrefBase {

public:
	//! Values returned from available_themes()

	struct available_theme {

		//! Identifier for the given theme
		std::string identifier;

		//! Its name or description, in UTF-8.
		std::string description;
	};

	//! Enumerate available themes.

	static std::vector<available_theme> available_themes();
};

LIBCXXW_NAMESPACE_END

#endif
