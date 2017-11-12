/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef themedimobj_h
#define themedimobj_h

#include "themedimfwd.H"
#include "connection_threadfwd.H"
#include "defaultthemefwd.H"
#include "x/w/screen.H"
#include "x/w/dim_arg.H"
#include <string>
#include <string_view>

LIBCXXW_NAMESPACE_START

//! A theme-specified dimension

//! \see themedim

class LIBCXX_HIDDEN themedimObj : virtual public obj {

	//! The name of the dimension
	const dim_arg dimname;

	//! Padding value in pixels.
	dim_t pixels_thread_only;

	//! Whether the dimension is width or height based
	const theme_width_or_height width_or_height;

 public:
	THREAD_DATA_ONLY(pixels);

	//! Constructor
	themedimObj(const dim_arg &dimname,
		    const ref<screenObj::implObj> &screen_impl,
		    theme_width_or_height width_or_height);

	//! Destructor
	~themedimObj();

	//! Recalculate padding.
	void initialize(IN_THREAD_ONLY,
			const ref<screenObj::implObj> &screen_impl);

	//! Recalculate padding.
	void theme_updated(IN_THREAD_ONLY,
			   const defaulttheme &new_theme);
};


LIBCXXW_NAMESPACE_END

#endif
