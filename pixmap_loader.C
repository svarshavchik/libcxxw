/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "pixmap_loader.H"
#include "screen.H"
#include "connection_thread.H"
#include "gc.H"

LIBCXXW_NAMESPACE_START

static inline auto create_image(dim_t w, dim_t h, depth_t depth,
				const screen &s)
{
	if (w == 0)
		w=1;

	if (h == 0)
		h=1;

	auto i=xcb_image_create_native(s->impl->thread->info->conn,
				       (dim_t::value_type)w,
				       (dim_t::value_type)h,
				       XCB_IMAGE_FORMAT_Z_PIXMAP,
				       (depth_t::value_type)depth,
				       NULL, ~0, NULL);

	if (!i)
		throw EXCEPTION("xcb_image_create_native failed");

	return i;
}

pixmap_loader::pixmap_loader(const pixmap &the_pixmap)
	: the_pixmap{the_pixmap},
	  width{the_pixmap->get_width()},
	  height{the_pixmap->get_height()},
	  pixmap_pictformat{the_pixmap->impl->drawable_pictformat},
	  image{create_image(width, height, pixmap_pictformat->depth,
			     the_pixmap->get_screen())}
{
	image->base=image->data=(decltype(image->data))calloc(1, image->size);
}

pixmap_loader::~pixmap_loader()
{
	xcb_image_destroy(image);
}

void pixmap_loader::flush()
{
	auto gc=the_pixmap->create_gc();

	xcb_image_put(the_pixmap->impl->get_screen()->impl->thread->info->conn,
		      the_pixmap->impl->drawable_id,
		      gc->impl->gc_id(),
		      image,
		      0, 0, 0);
}

LIBCXXW_NAMESPACE_END
