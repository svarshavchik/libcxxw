/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "fonts/fontcollection_impl.H"
#include "x/w/impl/fonts/current_fontcollection.H"
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
			    const font &font_spec,
			    const const_defaulttheme &font_theme)
	: font_spec_thread_only{font_spec},
	  font_screen{font_screen},
	  depth{depth},
	  fc_thread_only{create_fc(font_spec, font_theme)},
	  fc_public{fc_thread_only}
{
}

current_fontcollectionObj::~current_fontcollectionObj()=default;

void current_fontcollectionObj
::current_theme_updated(ONLY IN_THREAD,
			const const_defaulttheme &new_theme)
{
	fc_public=fc(IN_THREAD)=create_fc(font_spec(IN_THREAD),
					  new_theme);
}

fontcollection current_fontcollectionObj::create_fc(const font &font_spec,
						    const const_defaulttheme
						    &font_theme)
{
	return font_screen->create_fontcollection(font_spec,
						  depth,
						  font_theme);
}

fontcollection screenObj
::create_fontcollection(const font &font_spec,
			const depth_t depth)
{
	return create_fontcollection(font_spec, depth,
				     impl->current_theme.get());
}

fontcollection screenObj
::create_fontcollection(const font &font_spec,
			const depth_t depth,
			const const_defaulttheme &font_theme)
{
	auto dpi_scaled=
		std::round(dim_t::value_type
			   (impl->height_in_pixels())
			   * font_theme->themescale  / dim_t::value_type
			   (impl->height_in_millimeters())
			   * 25.4);

	auto p=impl->fc->create_pattern(font_spec, dpi_scaled);

	auto conn=get_connection()->impl;

	font_id_t key{p->unparse(), depth};

	mpobj<sorted_font_cache_t>::lock lock{conn->sorted_font_cache};

	return (*lock)->find_or_create
		(key,
		 [=, me=ref(this)]
		 {
			 return fontcollection::create
				 (ref<fontcollectionObj::implObj>::create
				  (key, p->match(), me,
				   me->impl->ft));
		 });
}

LIBCXXW_NAMESPACE_END
