/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "screen.H"
#include "picture.H"
#include "connection_thread.H"
#include "x/w/rgb.H"
#include "x/w/rgb_hash.H"
#include "x/w/picture.H"
#include "x/w/pictformat.H"
#include <x/weakunordered_multimap.H>

LIBCXXW_NAMESPACE_START

////////////////////////////////////////////////////////////////////////////
//
// Creates solid color fill pictures for a screen, of a particular colors.
//
// These are cached in a weak multimap. Requesting a solid color picture for
// the same color returns an existing object.

screen_solidcolorpictures::screen_solidcolorpictures()
	: map{map_t::create()}
{
}

screen_solidcolorpictures::~screen_solidcolorpictures()=default;

// Subclass of a picture implementation object constructs the picture
// via xcb_render_create_solid_fill().

class LIBCXX_HIDDEN solidColorPictureObj
	: public pictureObj::implObj {

 public:
	solidColorPictureObj(IN_THREAD_ONLY,
			     const rgb &color)
		:  implObj(IN_THREAD)
	{
		xcb_render_create_solid_fill(picture_conn()->conn,
					     picture_id(),
					     {
						     .red=color.r,
						     .green=color.g,
						     .blue=color.b,
						     .alpha=color.a
					     });
	}

	~solidColorPictureObj()
	{
		xcb_render_free_picture(picture_conn()->conn, picture_id());
	}
};

const_picture screenObj::create_solid_color_picture(const rgb &color) const
{
	return impl->screen_solidcolorpictures::map->
		find_or_create(color,
			       [this, &color]
			       {
				       auto impl=ref<solidColorPictureObj>
					       ::create(this->get_connection()
							->impl->thread,
							color);

				       return ref<pictureObj>::create(impl);
			       });
}

LIBCXXW_NAMESPACE_END
