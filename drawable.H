/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef drawable_H
#define drawable_H

#include "x/w/drawable.H"
#include <xcb/xproto.h>

namespace LIBCXX_NAMESPACE {
	namespace w {
#if 0
	}
}
#endif

//! Drawable implementation details.

class LIBCXX_HIDDEN drawableObj::implObj : virtual public obj {

 public:
	//! My ID.

	const xcb_drawable_t drawable_id;

	//! My pictformat

	const const_pictformat drawable_pictformat;

	implObj(xcb_drawable_t drawable_id,
		const const_pictformat drawable_pictformat);

	~implObj() noexcept;
};

#if 0
{
	{
#endif
	}
}

#endif
