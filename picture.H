/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef picture_h
#define picture_h

#include "x/w/picture.H"
#include "x/w/rectangle.H"
#include "xid_t.H"
#include <xcb/render.h>

LIBCXXW_NAMESPACE_START
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

	//! Composite RENDER operation

	void composite(const const_picture_internal &src,
		       const const_picture_internal &mask,
		       coord_t src_x,
		       coord_t src_y,
		       coord_t mask_x,
		       coord_t mask_y,
		       coord_t dst_x,
		       coord_t dst_y,
		       dim_t width,
		       dim_t height,
		       render_pict_op op=render_pict_op::op_src);

	//! Set REPEAT property.
	void repeat(render_repeat value);

	//! SetPictureClipRectangles RENDER operation.
	void set_clip_rectangle(const rectangle &r,
				coord_t x=0,
				coord_t y=0);

	//! SetPictureClipRectangles RENDER operation.
	void set_clip_rectangles(const rectarea &clipregion,
				 coord_t x=0,
				 coord_t y=0);

	//! Clear the clip area completely.
	void clear_clip();

	//! Fill a rectangle
	void fill_rectangle(coord_t x, coord_t y, dim_t width, dim_t height,
			    const rgb &color,
			    render_pict_op op=render_pict_op::op_src);
	//! Fill a rectangle
	void fill_rectangle(const rectangle &rectangle,
			    const rgb &color,
			    render_pict_op op=render_pict_op::op_src);
	//! Fill rectangles
	void fill_rectangles(const rectangle *rectangles,
			     size_t n_rectangles,
			     const rgb &color,
			     render_pict_op op=render_pict_op::op_src);

	//! Fill triangle strip
	void fill_tri_strip(const point *points,
			    size_t n_points,
			    const const_picture_internal &src,
			    render_pict_op op=render_pict_op::op_src,
			    coord_t src_x=0,
			    coord_t src_y=0);

	//! Fill triangle strip
	void fill_tri_strip(const point *points,
			    size_t n_points,
			    const const_picture_internal &src,
			    const const_pictformat &mask,
			    render_pict_op op=render_pict_op::op_src,
			    coord_t src_x=0,
			    coord_t src_y=0);

	//! Fill triangle strip
	void do_fill_tri_strip(const point *points,
			       size_t n_points,
			       const const_picture_internal &src,
			       xcb_render_pictformat_t mask,
			       render_pict_op op,
			       coord_t src_x,
			       coord_t src_y);

	//! Fill triangles
	void fill_triangles(const std::set<triangle> &triangles,
			    const const_picture_internal &src,
			    render_pict_op op=render_pict_op::op_src,
			    coord_t src_x=0,
			    coord_t src_y=0);

	//! Fill triangles
	void fill_triangles(const std::set<triangle> &triangles,
			    const const_picture_internal &src,
			    const const_pictformat &mask,
			    render_pict_op op=render_pict_op::op_src,
			    coord_t src_x=0,
			    coord_t src_y=0);

	//! Fill triangles
	void do_fill_triangles(const std::set<triangle> &triangles,
			       const const_picture_internal &src,
			       xcb_render_pictformat_t mask,
			       render_pict_op op,
			       coord_t src_x,
			       coord_t src_y);

	//! Fill a triangle fan.
	void fill_tri_fan(const point *points,
			  size_t n_points,
			  const const_picture_internal &src,
			  render_pict_op op=render_pict_op::op_src,
			  coord_t src_x=0,
			  coord_t src_y=0);

	//! Fill a triangle fan.
	void fill_tri_fan(const point *points,
			  size_t n_points,
			  const const_picture_internal &src,
			  const const_pictformat &mask,
			  render_pict_op op=render_pict_op::op_src,
			  coord_t src_x=0,
			  coord_t src_y=0);

	//! Fill triangle fan
	void do_fill_tri_fan(const point *points,
			     size_t n_points,
			     const const_picture_internal &src,
			     xcb_render_pictformat_t mask,
			     render_pict_op op,
			     coord_t src_x,
			     coord_t src_y);


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

rgb valid_gradient(const rgb_gradient &) LIBCXX_HIDDEN;

#define picture_included 1
#include "xid_t_connection_thread.H"

LIBCXXW_NAMESPACE_END

#endif
