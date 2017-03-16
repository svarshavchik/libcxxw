/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "picture.H"
#include "pictformat.H"
#include "xid_t.H"
#include "connection_thread.H"

LIBCXXW_NAMESPACE_START

#define TOVALUE(v) (((int32_t)v.integer << 16) | v.fraction)
#define POINT(p) { TOVALUE(p.x), TOVALUE(p.y) }

static void to_xcb_rectangles(std::vector<xcb_rectangle_t> &v,
			      const rectangle_set &rectangles)
{
	v.clear();
	v.reserve(rectangles.size());

	for (const auto &r : rectangles)
	{
		v.push_back({(coord_t::value_type)r.x,
					(coord_t::value_type)r.y,
					(dim_t::value_type)r.width,
					(dim_t::value_type)r.height});
	}
}


pictureObj::implObj::implObj(const connection_thread &thread)
	: picture_xid(thread)
{
}

pictureObj::implObj::~implObj()=default;

void pictureObj::implObj::set_clip_rectangle(const rectangle &r,
					     coord_t x,
					     coord_t y)
{
	rectangle_set s={ r };

	set_clip_rectangles(s, x, y);
}

void pictureObj::implObj::set_clip_rectangles(const rectangle_set &clipregion,
					      coord_t x,
					      coord_t y)
{
	std::vector<xcb_rectangle_t> v;

	to_xcb_rectangles(v, clipregion);

	xcb_render_set_picture_clip_rectangles(picture_conn()->conn,
					       picture_id(),
					       (coord_t::value_type)x,
					       (coord_t::value_type)y,
					       v.size(),
					       v.data());
}

void pictureObj::implObj::composite(const const_picture_internal &src,
				    coord_t src_x,
				    coord_t src_y,
				    coord_t dst_x,
				    coord_t dst_y,
				    dim_t width,
				    dim_t height,
				    render_pict_op op)
{
	xcb_render_composite(picture_conn()->conn,
			     (uint8_t)op,
			     src->picture_id(),
			     XCB_NONE,
			     picture_id(),
			     (coord_t::value_type)src_x,
			     (coord_t::value_type)src_y,
			     0, 0,
			     (coord_t::value_type)dst_x,
			     (coord_t::value_type)dst_y,
			     (dim_t::value_type)width,
			     (dim_t::value_type)height);
}

void pictureObj::implObj::fill_tri_strip(const point *points,
					 size_t n_points,
					 const const_picture_internal &src,
					 render_pict_op op,
					 coord_t src_x,
					 coord_t src_y)
{
	do_fill_tri_strip(points, n_points, src, XCB_NONE, op, src_x, src_y);
}

void pictureObj::implObj::fill_tri_strip(const point *points,
					 size_t n_points,
					 const const_picture_internal &src,
					 const const_pictformat &mask,
					 render_pict_op op,
					 coord_t src_x,
					 coord_t src_y)
{
	do_fill_tri_strip(points, n_points, src, mask->impl->id, op,
			  src_x, src_y);
}

void pictureObj::implObj::do_fill_tri_strip(const point *points,
					    size_t n_points,
					    const const_picture_internal &src,
					    xcb_render_pictformat_t mask,
					    render_pict_op op,
					    coord_t src_x,
					    coord_t src_y)
{
	if (n_points <= 0)
		return;


	xcb_render_pointfix_t strip[n_points];

	for (size_t i=0; i<n_points; ++i)
		strip[i]=xcb_render_pointfix_t(POINT(points[i]));

	xcb_render_tri_strip(picture_conn()->conn,
			     (uint8_t)op,
			     src->picture_id(),
			     picture_id(),
			     mask,
			     coord_t::value_type(src_x),
			     coord_t::value_type(src_y),
			     n_points,
			     &strip[0]);
}

////////////////////////////////////////////////////////////////////////////
//

pictureObj::implObj::fromDrawableObj
::fromDrawableObj(const connection_thread &thread,
		  xcb_drawable_t drawable,
		  xcb_render_pictformat_t format)
	: implObj(thread)
{
	xcb_render_create_picture(picture_conn()->conn,
				  picture_id(),
				  drawable,
				  format,
				  0,
				  nullptr);
}

pictureObj::implObj::fromDrawableObj::~fromDrawableObj()
{
	xcb_render_free_picture(picture_conn()->conn, picture_id());
}

LIBCXXW_NAMESPACE_END
