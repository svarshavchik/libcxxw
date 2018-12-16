/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"

#include "screen.H"
#include "ellipsiscache.H"
#include "screen_fontcaches.H"
#include "generic_window_handler.H"
#include "x/w/impl/fonts/current_fontcollection.H"
#include "x/w/impl/richtext/richtext.H"
#include "recycled_pixmaps.H"
#include "border_cache.H"
#include "x/w/border_arg_hash.H"
#include "defaulttheme.H"
#include "x/w/impl/background_colorobj.H"
#include "x/w/impl/fonts/current_fontcollection.H"
#include "x/w/impl/current_border_impl.H"

#include <x/weakunordered_multimap.H>
LIBCXXW_NAMESPACE_START

void screenObj::implObj::update_current_theme(ONLY IN_THREAD,
					      const defaulttheme &new_theme)
{
	// cxxwtheme set the CXXWTHEME property, this makes it
	// back to us, typically, before cxxwtheme gets around
	// to destroying the main window. Make sure to update
	// the screen's theme object only when needed.

	current_theme_t::lock lock{current_theme};

	if (!(*lock)->is_different_theme(new_theme))
		return;

	*lock=new_theme;

	// We hold this lock in place while updating all themes.
	// create_theme_font() and create_custom_font() also acquire this
	// lock before constructing a new theme. This way, the themes the
	// fonts are based on are all synchronized.
	//
	// textlabel_implObj is also constructed with this lock being held,
	// thus synchronizing the theme stored in textlabelObj::implObj.

	for (auto &cached_font:*fontcaches->custom_font_cache)
	{
		auto p=cached_font.second.getptr();

		if (p)
			p->current_theme_updated(IN_THREAD, new_theme);
	}

	for (auto &cached_font:*fontcaches->theme_font_cache)
	{
		auto p=cached_font.second.getptr();

		if (p)
			p->current_theme_updated(IN_THREAD, new_theme);
	}

	// We also hold this lock in place while calling theme_updated of
	// cached ellipsis.

	for (auto &cached_ellipsis:*ellipsiscaches->cache)
	{
		auto p=cached_ellipsis.second.getptr();

		if (p)
			p->theme_updated(IN_THREAD, new_theme);
	}

	// We also hold this lock in place while calling current_theme_updated
	// of all background_colors.

	for (auto &cached_bgcolor:
		     *recycled_pixmaps_cache->theme_background_color_cache)
	{
		auto p=cached_bgcolor.second.getptr();

		if (p)
			p->current_theme_updated(IN_THREAD, new_theme);
	}

	// We also hold this lock in place while calling current_theme_updated
	// of all borders.

	for (auto &cached_border: *screen_border_cache->map)
	{
		auto p=cached_border.second.getptr();

		if (p)
			p->current_theme_updated(IN_THREAD, new_theme);
	}
}

LIBCXXW_NAMESPACE_END
