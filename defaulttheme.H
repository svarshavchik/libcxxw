/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef defaulttheme_h
#define defaulttheme_h

#include <x/ref.H>
#include "defaultthemefwd.H"
#include "defaultthemeobj.H"

LIBCXXW_NAMESPACE_START

//! Base class for loaded themes

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::defaulttheme::base

class defaultthemeBase : public ptrrefBase {

public:

	//! Convenience typedef
	typedef defaultthemeObj::config config;

	//! Load the theme configuration.
	static config get_config(const xcb_screen_t *screen_0,
				 const connection_thread &thread);
};


LIBCXXW_NAMESPACE_END

#endif
