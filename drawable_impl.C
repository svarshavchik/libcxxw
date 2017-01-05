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
			      const xcb_drawable_t drawable_id,
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

// Subclass of pictureObj::implObj that stores a ref to the original drawable,
// to make sure it does not get destroyed until the picture is destroyed.

class LIBCXX_HIDDEN drawablePictureObj
	: public pictureObj::implObj::fromDrawableObj {

 public:
	const ref<drawableObj::implObj> drawable;

	drawablePictureObj(const connection_thread &thread,
			   const ref<drawableObj::implObj> &drawable,
			   xcb_render_pictformat_t format)
		: pictureObj::implObj::fromDrawableObj(thread,
						       drawable->drawable_id,
						       format),
		drawable(drawable)

	{
	}

	~drawablePictureObj()=default;
};

picture drawableObj::implObj::create_picture()
{
	auto impl=ref<pictureObj::implObj::fromDrawableObj>
		::create(thread_,
			 drawable_id,
			 drawable_pictformat->impl->id);
	return picture::create(impl);
}

LIBCXXW_NAMESPACE_END
