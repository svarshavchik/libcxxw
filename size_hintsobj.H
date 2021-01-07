/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_size_hintsobj_h
#define x_w_size_hintsobj_h

#include "size_hintsfwd.H"
#include "x/w/types.H"
#include "x/w/impl/connection_threadfwd.H"
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see size_hints

class size_hintsObj: virtual public obj {

	//! Width increment
	dim_t width_increment_thread_only=1;

	//! Height increment
	dim_t height_increment_thread_only=1;

	//! Base width

	//! The minimum width indicates a base width of these many columns.
	dim_t base_width_thread_only=0;

	//! Base height
	//! The minimum height indicates a base height of these many columns.
	dim_t base_height_thread_only=0;
 public:

	THREAD_DATA_ONLY(width_increment);
	THREAD_DATA_ONLY(height_increment);
	THREAD_DATA_ONLY(base_width);
	THREAD_DATA_ONLY(base_height);

	//! Constructor
	size_hintsObj();

	//! Destructor
	~size_hintsObj();
};

LIBCXXW_NAMESPACE_END

#endif
