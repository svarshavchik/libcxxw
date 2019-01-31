/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef pixmap_h
#define pixmap_h

#include "x/w/pixmap.H"
#include "x/w/types.H"
#include "x/w/screenfwd.H"
#include "drawable.H"
#include "xid_t.H"

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN pixmapObj::implObj : public xidObj,
					 public drawableObj::implObj {


 public:
	implObj(const const_pictformat &pixmap_pictformat,
		const screen &screenref,
		dim_t width,
		dim_t height);

	const screen screenref;
	const dim_t width;
	const dim_t height;

	inline xcb_pixmap_t pixmap_id() const { return drawable_id; }

	~implObj();

	//! Points of interest in this pixmap.

	//! Set in icon_cache.C when the pixmap is created from an SXG file,
	//! and not modified after the fact.
	pixmap_points_of_interest_t points_of_interest;

	////////////////////////////////////////////////////////////////////
	//
	// Inherited from drawableObj::implObj

	//! Return my screen
	screen get_screen() override;

	//! Return my screen
	const_screen get_screen() const override;

	//! Return my width
	dim_t get_width() const override;

	//! Return my height
	dim_t get_height() const override;
};

LIBCXXW_NAMESPACE_END

#endif
