/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_pixmapobj_h
#define x_w_pixmapobj_h

#include <x/w/pixmapfwd.H>
#include <x/w/drawableobj.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Generic window facilities.

//! \see pixmap

class pixmapObj : public drawableObj {

public:
	class implObj;

	//! Internal implementation details

	const ref<implObj> impl;

	//! Points of interest in this pixmap.

	const pixmap_points_of_interest_t &points_of_interest;

	//! Constructor
	pixmapObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~pixmapObj() LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
