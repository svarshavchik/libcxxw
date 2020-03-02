/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_themedimobj_h
#define x_w_impl_themedimobj_h

#include <x/w/impl/themedimfwd.H>
#include "x/w/impl/connection_threadfwd.H"
#include "x/w/defaultthemefwd.H"
#include "x/w/screen.H"
#include "x/w/dim_arg.H"
#include <string>
#include <string_view>

LIBCXXW_NAMESPACE_START

//! A theme-specified dimension

//! \see themedim

class themedimObj : virtual public obj {

	//! The name of the dimension
	dim_arg dimname;

	//! How big it is, in pixels.
	dim_t pixels_thread_only;

	//! Whether the dimension is width or height based
	const themedimaxis width_or_height;

 public:
	THREAD_DATA_ONLY(pixels);

	//! Constructor
	themedimObj(const dim_arg &dimname,
		    const ref<screenObj::implObj> &screen_impl,
		    themedimaxis width_or_height);

	//! Constructor
	themedimObj(const themedim_element_init &init);

	//! Destructor
	~themedimObj();

	//! Recalculate the size in pixels.
	void initialize(ONLY IN_THREAD,
			const ref<screenObj::implObj> &screen_impl);

	//! Recalculate the size in pixels.
	void theme_updated(ONLY IN_THREAD,
			   const const_defaulttheme &new_theme);

	//! Update the dimension.
	void update(ONLY IN_THREAD,
		    const dim_arg &new_dimname,
		    const const_defaulttheme &current_theme);
};


LIBCXXW_NAMESPACE_END

#endif
