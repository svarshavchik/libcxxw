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

LIBCXXW_NAMESPACE_START

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

LIBCXXW_NAMESPACE_END
