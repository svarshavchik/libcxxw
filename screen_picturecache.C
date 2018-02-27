/*
** Copyright 2017-2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "screen_picturecache.H"
#include "picture.H"
#include "xid_t.H"
#include "connection_thread.H"
#include "screen.H"
#include "x/w/rgb_hash.H"
#include <x/weakunordered_multimap.H>
#include <xcb/xproto.h>

LIBCXXW_NAMESPACE_START

////////////////////////////////////////////////////////////////////////////
//
// Creates solid color fill pictures for a screen, of a particular colors.
//
// These are cached in a weak multimap. Requesting a solid color picture for
// the same color returns an existing object.

screen_picturecacheObj::screen_picturecacheObj()
	: solid_colors{solid_color_map_t::create()},
	  linear_gradients{linear_gradient_map_t::create()}
{
}

screen_picturecacheObj::~screen_picturecacheObj()=default;

namespace {
#if 0
}
#endif

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

#if 0
{
#endif
}

const_picture screenObj::create_solid_color_picture(const rgb &color) const
{
	return impl->create_solid_color_picture(color);
}

const_picture screenObj::implObj::create_solid_color_picture(const rgb &color)
{
	return picturecache->solid_colors->
		find_or_create(color,
			       [this, &color]
			       {
				       auto impl=ref<solidColorPictureObj>
					       ::create(thread, color);

				       return ref<pictureObj>::create(impl);
			       });
}

LIBCXXW_NAMESPACE_END
