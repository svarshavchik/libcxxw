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

pictureObj::implObj::implObj(xcb_render_picture_t picture_id)
	: picture_id(picture_id)
{
}

pictureObj::implObj::~implObj()=default;

////////////////////////////////////////////////////////////////////////////
//

pictureObj::implObj::fromDrawableObj
::fromDrawableObj(const connection_thread &thread,
		  xcb_drawable_t drawable,
		  xcb_render_pictformat_t format)
	: pictureObj::implObj::picture_xid(thread),
	implObj(pictureObj::implObj::picture_xid::get_picture_xid())
{
	xcb_render_create_picture(thread->info->conn,
				  get_picture_xid(),
				  drawable,
				  format,
				  0,
				  nullptr);
}

pictureObj::implObj::fromDrawableObj::~fromDrawableObj()
{
	xcb_render_free_picture(picture_xid_obj.conn()->conn,
				get_picture_xid());
}

LIBCXXW_NAMESPACE_END
