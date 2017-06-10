/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef themedimobj_h
#define themedimobj_h

#include "themedimfwd.H"
#include "connection_threadfwd.H"
#include "x/w/screen.H"
#include <string>

LIBCXXW_NAMESPACE_START

//! A theme-specified dimension

//! \see themedim

class LIBCXX_HIDDEN themedimObj : virtual public obj {

	//! The name of the dimension
	const std::string dimname;

	//! Padding value in pixels.
	dim_t pixels_thread_only;

 public:
	THREAD_DATA_ONLY(pixels);

	//! Constructor
	themedimObj(const std::string &dimname,
		    const ref<screenObj::implObj> &screen_impl);

	//! Destructor
	~themedimObj();

	//! Recalculate padding.
	void theme_updated(IN_THREAD_ONLY,
			   const ref<screenObj::implObj> &screen_impl);
};


LIBCXXW_NAMESPACE_END

#endif
