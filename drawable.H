/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef drawable_H
#define drawable_H

#include "x/w/drawable.H"
#include "x/w/pictformatfwd.H"
#include "x/w/picturefwd.H"
#include "xid_t.H"

#include <xcb/xproto.h>

namespace LIBCXX_NAMESPACE {
	namespace w {
#if 0
	}
}
#endif

//! Drawable implementation details.

class LIBCXX_HIDDEN drawableObj::implObj : virtual public obj {

	const connection_thread thread_;

 public:

	//! My xcb_drawable_t

	const xcb_drawable_t drawable_id;

	//! My pictformat

	const const_pictformat drawable_pictformat;

	//! Constructor

	implObj(//! My connection thread

		const connection_thread &thread_,

		//! My drawable
		const xcb_drawable_t drawable_id,

		//! Drawable's pixtformat.
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
