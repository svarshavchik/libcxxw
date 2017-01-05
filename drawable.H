/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef drawable_H
#define drawable_H

#include "x/w/drawable.H"
#include "x/w/picturefwd.H"
#include "connection_info.H"
#include "connection_threadfwd.H"

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

	//! My connection
	const connection_thread &thread_;

	//! My ID.

	const xcb_drawable_t drawable_id;

	//! My pictformat

	const const_pictformat drawable_pictformat;

	implObj(const connection_thread &thread_,
		xcb_drawable_t drawable_id,
		const const_pictformat drawable_pictformat);

	~implObj();

	//! Create a new picture.

	picture create_picture();
};

#if 0
{
	{
#endif
	}
}

#endif
