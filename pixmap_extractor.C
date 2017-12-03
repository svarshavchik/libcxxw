/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "pixmap_extractor.H"
#include "screen.H"
#include "connection_thread.H"

LIBCXXW_NAMESPACE_START

static inline auto get_image(xcb_drawable_t drawable,
			     dim_t w, dim_t h,
			     const const_screen &s)
{
	if (w == 0)
		w=1;

	if (h == 0)
		h=1;

	auto i=xcb_image_get(s->impl->thread->info->conn,
			     drawable,
			     0, 0,
			     (dim_t::value_type)w,
			     (dim_t::value_type)h,
			     ~0,
			     XCB_IMAGE_FORMAT_Z_PIXMAP);

	if (!i)
		throw EXCEPTION("xcb_image_get failed");

	return i;
}

static inline auto create_indexed_lookup(const const_pictformat &pf)
{
	std::unordered_map<uint32_t, rgb> indexed_lookup;

	for (const auto &i:pf->color_indexes)
		indexed_lookup.emplace(i.index, i.color);
	return indexed_lookup;
}

pixmap_extractor::pixmap_extractor(const const_pixmap &the_pixmap)
	: the_pixmap{the_pixmap},
	  width{the_pixmap->get_width()},
	  height{the_pixmap->get_height()},
	  pixmap_pictformat{the_pixmap->impl->drawable_pictformat},
	  indexed_lookup{create_indexed_lookup(pixmap_pictformat)},
	  image{get_image(the_pixmap->impl->drawable_id,
			  width, height,
			  the_pixmap->get_screen())}
{
}

pixmap_extractor::~pixmap_extractor()
{
	xcb_image_destroy(image);
}

LIBCXXW_NAMESPACE_END
