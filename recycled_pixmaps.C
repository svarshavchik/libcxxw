/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "recycled_pixmaps.H"
#include "scratch_buffer.H"
#include "background_color.H"
#include "screen.H"
#include "x/w/pictformat.H"
#include "pixmap.H"
#include "x/w/picture.H"
#include <x/ref.H>
#include <x/refptr_hash.H>
#include <x/weakunordered_multimap.H>

LIBCXXW_NAMESPACE_START

recycled_pixmapsObj::recycled_pixmapsObj()
	: scratch_buffer_cache{scratch_buffer_cache_t::create()},
	  theme_background_color_cache{theme_background_color_cache_t::create()}
{
}

recycled_pixmapsObj::~recycled_pixmapsObj()=default;

scratch_buffer screenObj::create_scratch_buffer(const std::string &identifier,
						const const_pictformat &pf,
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
			const const_pictformat &pf,
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

////////////////////////////////////////////////////////////////////////////
//
// Implements background_color object as a color specified by the theme.
//
// The current background color picture is cached. Each call to
// get_current_color() retrieves the color from theme, which is just an
// unordered_map lookup, compares it to the cached color, and creates a
// new picture, if necessary.

class LIBCXX_HIDDEN theme_background_colorObj : public background_colorObj {

	struct s_info {

		rgb current_rgb;
		const_picture current_color;
	};

	typedef mpobj<s_info> info_t;

	const std::string theme_color;
	const ref<screenObj::implObj> screen;

	info_t info;

 public:

	// The constructor gets initializes with the current background color

	theme_background_colorObj(const std::string &theme_color,
				  const rgb &current_rgb,
				  const const_picture &current_picture,
				  const ref<screenObj::implObj> &screen)
		: theme_color(theme_color),
		screen(screen),
		info{current_rgb, current_picture}
		{
		}

	~theme_background_colorObj()=default;

	const_picture get_current_color() override
	{
		info_t::lock lock(info);

		// Check the current theme color. Did it change?

		auto current_rgb=screen->get_theme_color(theme_color,
							 lock->current_rgb);

		if (current_rgb != lock->current_rgb)
		{
			// Create a new color.

			lock->current_color=screen->create_solid_color_picture
				(current_rgb);

			lock->current_rgb=current_rgb;
		}

		return lock->current_color;
	}
};

background_color screenObj::implObj
::create_background_color(const std::experimental::string_view &color_name,
			  const rgb &default_value)
{
	std::string color_name_str{color_name};

	auto current_rgb=get_theme_color(color_name, default_value);

	return recycled_pixmaps_cache->theme_background_color_cache
		->find_or_create
		(color_name_str,
		 [&,this]
		 {
			 return ref<theme_background_colorObj>
				 ::create(color_name_str,
					  current_rgb,
					  this->create_solid_color_picture
					  (current_rgb),
					  ref<screenObj::implObj>(this));
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
		+ std::hash<const_pictformat>()(k.pf);
}

LIBCXXW_NAMESPACE_END
