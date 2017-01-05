/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "drawable.H"
#include "picture.H"
#include "pictformat.H"
#include "connection_thread.H"
#include "x/w/pictformat.H"

LIBCXXW_NAMESPACE_START

drawableObj::implObj::implObj(const connection_thread &thread_,
			      xcb_drawable_t drawable_id,
			      const const_pictformat drawable_pictformat)
	: thread_(thread_),
	  drawable_id(drawable_id),
	  drawable_pictformat(drawable_pictformat)
{
}

drawableObj::implObj::~implObj()=default;

////////////////////////////////////////////////////////////////////////////
//
// Create a picture for a drawable.

// Subclass of a picture implementation object constructs the picture
// via xcb_render_create_picture().

class LIBCXX_HIDDEN drawablePictureObj : public pictureObj::implObj {

 public:
	drawablePictureObj(const connection_thread &thread_,
			   xcb_drawable_t drawable,
			   xcb_render_pictformat_t format)
		: implObj(thread_)
	{
		xcb_render_create_picture(conn()->conn,
					  id(),
					  drawable,
					  format,
					  0,
				  nullptr);
	}

	~drawablePictureObj()
	{
		xcb_render_free_picture(conn()->conn, id());
	}
};

picture drawableObj::implObj::create_picture()
{
	auto impl=ref<drawablePictureObj>
		::create(thread_,
			 drawable_id,
			 drawable_pictformat->impl->id);
	return picture::create(impl);
}

LIBCXXW_NAMESPACE_END
