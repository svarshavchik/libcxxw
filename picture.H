/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef picture_h
#define picture_h

#include "x/w/picture.H"
#include "x/w/rectangle.H"
#include "xid_t.H"
#include <xcb/render.h>

LIBCXXW_NAMESPACE_START

typedef ref<pictureObj::implObj> picture_internal;

typedef ref<pictureObj::implObj> const_picture_internal;

//! picture internal implementation details.

class LIBCXX_HIDDEN pictureObj::implObj : virtual public obj {
 public:
	//! The allocated XID.
	const new_xid picture_xid;

	//! My id

	inline xcb_render_picture_t picture_id() const
	{
		return picture_xid.id_;
	}

	//! My connection

	inline auto picture_conn() const;

	//! The constructor allocates a new XID for the picture.
	implObj(const connection_thread &thread);

	//! The destructor deallocates the XID.
	~implObj();

	//! Composite RENDER operation

	void composite(const const_picture_internal &src,
		       coord_t src_x,
		       coord_t src_y,
		       coord_t dst_x,
		       coord_t dst_y,
		       dim_t width,
		       dim_t height,
		       render_pict_op op=render_pict_op::op_src);

	//! Composite RENDER operation

	inline void composite(const const_picture_internal &src,
			      coord_t src_x,
			      coord_t src_y,
			      const rectangle &rect,
			      render_pict_op op=render_pict_op::op_src)
	{
		composite(src, src_x, src_y,
			  rect.x,
			  rect.y,
			  rect.width,
			  rect.height,
			  op);
	}

	//! SetPictureClipRectangles RENDER operation.
	void set_clip_rectangle(const rectangle &r,
				coord_t x=0,
				coord_t y=0);

	//! SetPictureClipRectangles RENDER operation.
	void set_clip_rectangles(const rectangle_set &clipregion,
				 coord_t x=0,
				 coord_t y=0);

	class fromDrawableObj;
};

//! Create a picture from a a drawable.

//! Subclass of a picture implementation object allocates a new
//! a new XID for a drawable's picture, then constructs the picture
//! via xcb_render_create_picture().

class LIBCXX_HIDDEN pictureObj::implObj::fromDrawableObj
	: public pictureObj::implObj {
 public:
	//! The constructor invokes xcb_render_create_picture()

	fromDrawableObj(const connection_thread &thread,
			xcb_drawable_t drawable,
			xcb_render_pictformat_t format);

	//! The destructor invokes xcb_render_free_picture()

	~fromDrawableObj();
};

#define picture_included 1
#include "xid_t_connection_thread.H"

LIBCXXW_NAMESPACE_END

#endif
