/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "fonts/fontcollection_impl.H"
#include "fonts/current_fontcollection.H"
#include "fonts/fontpattern.H"
#include "fonts/fontid_t_hash.H"
#include "fonts/fontconfig.H"
#include "screen.H"
#include "connection.H"
#include "connection_thread.H"
#include "defaulttheme.H"

LIBCXXW_NAMESPACE_START

current_fontcollectionObj
::current_fontcollectionObj(const screen &font_screen,
			    depth_t depth,
			    const font &font_spec)
	: current_fontcollectionObj(font_screen, depth, font_spec,
				    *current_theme_t::lock{
					    font_screen->impl->current_theme
						    })
{
}

current_fontcollectionObj
::current_fontcollectionObj(const screen &font_screen,
			    depth_t depth,
			    const font &font_spec,
			    const defaulttheme &font_theme)
	: font_spec_thread_only{font_spec},
	  font_theme{font_theme},
	  font_screen{font_screen},
	  depth{depth},
	  fc_thread_only{create_fc(font_spec)}
{
}

current_fontcollectionObj::~current_fontcollectionObj()=default;

void current_fontcollectionObj::theme_updated(ONLY IN_THREAD,
					      const defaulttheme &new_theme)
{
	if (new_theme == font_theme)
		return; // Hasn't changed.

	font_theme=new_theme;
	theme_was_really_updated(IN_THREAD);

	fc(IN_THREAD)=create_fc(font_spec(IN_THREAD));
}

fontcollection current_fontcollectionObj::create_fc(const font &font_spec)
{
	auto screen_impl=font_screen->impl;

	auto dpi_scaled=
		std::round(dim_t::value_type
			   (screen_impl->height_in_pixels())
			   * font_theme->themescale  / dim_t::value_type
			   (screen_impl->height_in_millimeters())
			   * 25.4);

	auto p=font_screen->impl->fc
		->create_pattern(font_spec, dpi_scaled);

	auto conn=font_screen->get_connection()->impl;

	font_id_t key{p->unparse(), depth};

	mpobj<sorted_font_cache_t>::lock lock{conn->sorted_font_cache};

	return (*lock)->find_or_create
		(key,
		 [&, this]
		 {
			 return fontcollection::create
				 (ref<fontcollectionObj::implObj>::create
				  (key, p->match(), font_screen,
				   font_screen->impl->ft));
		 });
}

void current_fontcollectionObj::theme_was_really_updated(ONLY IN_THREAD)
{
}

LIBCXXW_NAMESPACE_END
