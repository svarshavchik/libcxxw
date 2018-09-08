/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "picture.H"
#include "pictformat.H"
#include "pixmap.H"
#include "x/w/pictformat.H"
#include "x/w/rectangle.H"
#include "x/w/values_and_mask.H"
#include "xid_t.H"
#include "connection_thread.H"
#include "messages.H"

#include <x/chrcasecmp.H>

LIBCXXW_NAMESPACE_START

#include "pic_constants.inc.C"

std::string picture::base::render_pict_op_to_string(render_pict_op v)
{
	return pic_render_pict_op_to_string(v);
}

render_pict_op
picture::base::render_pict_op_from_string(const std::string_view &s)
{
	return pic_render_pict_op_from_string(s);
}

std::string picture::base::render_repeat_tostring(render_repeat v)
{
	return pic_render_repeat_to_string(v);
}

render_repeat picture::base
::render_repeat_from_string(const std::string_view &s)
{
	return pic_render_repeat_from_string(s);
}

pictureObj::pictureObj(const ref<implObj> &impl)
	: impl(impl)
{
}

pictureObj::~pictureObj()=default;

void pictureObj::composite(const const_picture &src,
			   coord_t src_x,
			   coord_t src_y,
			   const rectangle &rect,
			   render_pict_op op)
{
	composite(src, src_x, src_y,
		  rect.x,
		  rect.y,
		  rect.width,
		  rect.height,
		  op);
}

void pictureObj::composite(const const_picture &src,
			   coord_t src_x,
			   coord_t src_y,
			   coord_t dst_x,
			   coord_t dst_y,
			   dim_t width,
			   dim_t height,
			   render_pict_op op)
{
	impl->composite(src->impl, src_x, src_y, dst_x, dst_y, width,
			height, op);
}

void pictureObj::composite(const const_picture &src,
			   const const_picture &mask,
			   coord_t src_x,
			   coord_t src_y,
			   coord_t mask_x,
			   coord_t mask_y,
			   coord_t dst_x,
			   coord_t dst_y,
			   dim_t width,
			   dim_t height,
			   render_pict_op op)
{
	impl->composite(src->impl, mask->impl, src_x, src_y,
			mask_x, mask_y,
			dst_x, dst_y, width, height, op);
}

void pictureObj::repeat(render_repeat value)
{
	impl->repeat(value);
}

void pictureObj::set_clip_rectangles(const rectarea &clipregion,
				     coord_t x,
				     coord_t y)
{
	impl->set_clip_rectangles(clipregion, x, y);
}

void pictureObj::clear_clip()
{
	impl->clear_clip();
}

pictureObj::clip_mask::clip_mask(const picture &clipped_picture,
				 const pixmap &picture_clip_mask,
				 coord_t clip_x_origin,
				 coord_t clip_y_origin)
	: clipped_picture(clipped_picture),
	  picture_clip_mask(picture_clip_mask)
{
	if (picture_clip_mask->get_pictformat()->depth != 1)
		throw EXCEPTION(_("Clip mask is not a 1-bit deep pixmap"));

	values_and_mask params(XCB_RENDER_CP_CLIP_MASK,
			       picture_clip_mask->impl->pixmap_id(),
			       XCB_RENDER_CP_CLIP_X_ORIGIN,
			       (coord_t::value_type)clip_x_origin,
			       XCB_RENDER_CP_CLIP_Y_ORIGIN,
			       (coord_t::value_type)clip_y_origin);

	xcb_render_change_picture(picture_clip_mask->impl->conn()->conn,
				  clipped_picture->impl->picture_id(),
				  params.mask(), params.values().data());
}

pictureObj::clip_mask::~clip_mask()
{
	xcb_pixmap_t id=XCB_NONE;

	xcb_render_change_picture(picture_clip_mask->impl->conn()->conn,
				  clipped_picture->impl->picture_id(),
				  XCB_RENDER_CP_CLIP_MASK, &id);
}

pictureObj::rectangular_clip_mask
::rectangular_clip_mask(const picture &clipped_picture,
			const rectarea &rectangles,
			coord_t x,
			coord_t y)
	: clipped_picture(clipped_picture)
{
	clipped_picture->impl->set_clip_rectangles(rectangles, x, y);
}

pictureObj::rectangular_clip_mask::~rectangular_clip_mask()
{
	xcb_pixmap_t id=XCB_NONE;

	xcb_render_change_picture(clipped_picture->impl->picture_conn()->conn,
				  clipped_picture->impl->picture_id(),
				  XCB_RENDER_CP_CLIP_MASK, &id);
}

void pictureObj::fill_rectangle(const rectangle &r,
				const rgb &color,
				render_pict_op op)
{
	impl->fill_rectangles(&r, 1, color, op);
}

void pictureObj::fill_rectangles(const rectangle *rectangles,
				 size_t n_rectangles,
				 const rgb &color,
				 render_pict_op op)
{
	impl->fill_rectangles(rectangles, n_rectangles, color, op);
}

void pictureObj::fill_triangles(const std::set<triangle> &triangles,
				const const_picture &src,
				render_pict_op op,
				coord_t src_x,
				coord_t src_y)
{
	impl->fill_triangles(triangles, src->impl, op, src_x, src_y);
}

void pictureObj::fill_triangles(const std::set<triangle> &triangles,
				const const_picture &src,
				const const_pictformat &mask,
				render_pict_op op,
				coord_t src_x,
				coord_t src_y)
{
	impl->fill_triangles(triangles, src->impl, mask, op, src_x, src_y);
}

void pictureObj::fill_tri_strip(const point *points,
				size_t n_points,
				const const_picture &src,
				render_pict_op op,
				coord_t src_x,
				coord_t src_y)
{
	impl->fill_tri_strip(points, n_points, src->impl, op, src_x, src_y);
}

void pictureObj::fill_tri_strip(const point *points,
				size_t n_points,
				const const_picture &src,
				const const_pictformat &mask,
				render_pict_op op,
				coord_t src_x,
				coord_t src_y)
{
	impl->fill_tri_strip(points, n_points, src->impl, mask, op,
			     src_x, src_y);
}

void pictureObj::fill_tri_fan(const point *points,
			      size_t n_points,
			      const const_picture &src,
			      render_pict_op op,
			      coord_t src_x,
			      coord_t src_y)
{
	impl->fill_tri_fan(points, n_points, src->impl, op, src_x, src_y);
}

void pictureObj::fill_tri_fan(const point *points,
			      size_t n_points,
			      const const_picture &src,
			      const const_pictformat &mask,
			      render_pict_op op,
			      coord_t src_x,
			      coord_t src_y)
{
	impl->fill_tri_fan(points, n_points, src->impl, mask, op,
			   src_x, src_y);
}

LIBCXXW_NAMESPACE_END
