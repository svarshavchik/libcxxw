/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "drawable.H"
#include "picture.H"
#include "pixmap.H"
#include "pictformat.H"
#include "screen.H"
#include "connection.H"
#include "connection_thread.H"
#include "x/w/pictformat.H"

LIBCXXW_NAMESPACE_START

drawableObj::implObj::implObj(const connection_thread &thread_,
			      const xcb_drawable_t drawable_id,
			      const const_pictformat &drawable_pictformat)
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


pixmap drawableObj::implObj::create_pixmap(dim_t width,
					   dim_t height)
{
	return create_pixmap(width, height, drawable_pictformat);
}

pixmap drawableObj::implObj::create_pixmap(dim_t width,
					   dim_t height,
					   depth_t depth)
{
	return create_pixmap(width, height,
			     get_screen()->get_connection()->impl
			     ->render_info.find_alpha_pictformat_by_depth
			     (depth));
}

pixmap drawableObj::implObj::create_pixmap(dim_t width,
					   dim_t height,
					   const const_pictformat &pf)
{
	return get_screen()->create_pixmap(pf, width, height);
}

pixmap screenObj::create_pixmap(const const_pictformat &pf,
				dim_t width,
				dim_t height)
{
	if (width == dim_t::infinite() ||
	    height == dim_t::infinite())
		throw EXCEPTION("Internal error, invalid scratch pixmap size");

	return pixmap::create(ref<pixmapObj::implObj>
			      ::create(pf,
				       ref(this),
				       width, height));
}

depth_t drawableObj::implObj::get_depth() const
{
	return drawable_pictformat->depth;
}

depth_t drawableObj::implObj::font_alpha_depth() const
{
	auto d=get_depth();

        switch (depth_t::value_type(d)) {
        case 1:
        case 2:
        case 4:
                return d;
        default:
                break;
        }
        return 8;
}

LIBCXXW_NAMESPACE_END
