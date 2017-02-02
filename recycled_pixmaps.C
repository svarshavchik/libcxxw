/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "recycled_pixmaps.H"
#include "scratch_buffer.H"
#include "screen.H"
#include "x/w/pictformat.H"
#include "pixmap.H"
#include "x/w/picture.H"
#include <x/ref.H>
#include <x/refptr_hash.H>
#include <x/weakunordered_multimap.H>

LIBCXXW_NAMESPACE_START

recycled_pixmapsObj::recycled_pixmapsObj()
	: scratch_buffer_cache(scratch_buffer_cache_t::create())
{
}

recycled_pixmapsObj::~recycled_pixmapsObj()=default;

scratch_buffer screenObj::create_scratch_buffer(const std::string &identifier,
						const pictformat &pf,
						dim_t initial_width,
						dim_t initial_height)
{
	return impl->create_scratch_buffer(screen(this),
					   identifier,
					   pf,
					   initial_width,
					   initial_height);
}

scratch_buffer screenObj::implObj
::create_scratch_buffer(const screen &public_object,
			const std::string &identifier,
			const pictformat &pf,
			dim_t initial_width,
			dim_t initial_height)
{
	return recycled_pixmaps_cache->scratch_buffer_cache
		->find_or_create({ identifier, pf },
				 [&]
				 {
					 auto pmi=ref<pixmapObj::implObj>
						 ::create(pf, public_object,
							  initial_width,
							  initial_height);

					 auto i=ref<scratch_bufferObj::implObj>
						 ::create(pixmap::create(pmi));

					 return scratch_buffer::create(i);
				 });
}


/////////////////////////////////////////////////////////////////////////////

bool recycled_pixmapsObj
::scratch_buffer_key::operator==(const scratch_buffer_key &o) const
{
	return identifier == o.identifier && pf == o.pf;
}

size_t recycled_pixmapsObj
::scratch_buffer_key_hash::operator()(const scratch_buffer_key &k) const
{
	return std::hash<std::string>()(k.identifier)
		+ std::hash<pictformat>()(k.pf);
}

LIBCXXW_NAMESPACE_END
