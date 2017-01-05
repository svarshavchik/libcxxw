/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef picture_h
#define picture_h

#include "x/w/picture.H"

#include "xid_t.H"
#include <xcb/render.h>

LIBCXXW_NAMESPACE_START

//! picture internal implementation details.

class LIBCXX_HIDDEN pictureObj::implObj : virtual public obj {
 public:
	class picture_xid;

	const xcb_render_picture_t picture_id;

	implObj(xcb_render_picture_t picture_id);

	~implObj();

	class fromDrawableObj;
};

//! A wrapper class to allocate an XID for a picture.

class LIBCXX_HIDDEN pictureObj::implObj::picture_xid {

public:
	//! The allocated XID.
	const xid_t<xcb_render_picture_t> picture_xid_obj;

	//! Constructor
	template<typename Arg>
	picture_xid(Arg &&arg)
		: picture_xid_obj(std::forward<Arg>(arg))
	{
	}

	//! Return the allocated picture_id

	auto get_picture_xid() const
	{
		return picture_xid_obj.id();
	}
};

//! Create a picture from a a drawable.

//! Subclass of a picture implementation object allocates a new
//! a new XID for a drawable's picture, then constructs the picture
//! via xcb_render_create_picture().

class LIBCXX_HIDDEN pictureObj::implObj::fromDrawableObj
	: public pictureObj::implObj::picture_xid,
	  public pictureObj::implObj {
 public:
	//! The constructor invokes xcb_render_create_picture()

	fromDrawableObj(const connection_thread &thread,
			xcb_drawable_t drawable,
			xcb_render_pictformat_t format);

	//! The destructor invokes xcb_render_free_picture()

	~fromDrawableObj();
};

LIBCXXW_NAMESPACE_END

#endif
