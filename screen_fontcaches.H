/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef screen_fontcaches_h
#define screen_fontcaches_h

#include "screen_fontcachesfwd.H"
#include "x/w/fontfwd.H"
#include "x/w/types.H"
#include "x/w/screenfwd.H"
#include "x/w/impl/fonts/current_fontcollectionfwd.H"
#include <x/weakunordered_multimapfwd.H>
#include <x/obj.H>
#include <x/ref.H>
#include <string>

LIBCXXW_NAMESPACE_START

//! A cache of \ref current_fontcollection "current font collections" for this screen.

class LIBCXX_HIDDEN screen_fontcachesObj : virtual public obj {

 public:

	//! Custom fonts are cached by their font description+alpha depth.
	typedef std::pair<font, depth_t> custom_font_cache_key_t;

	//! Custom font cache container.
	typedef x::weakunordered_multimap<custom_font_cache_key_t,
					  current_fontcollectionObj>
		custom_font_cache_t;

	//! Custom fonts.
	const custom_font_cache_t custom_font_cache;

	//! find_or_create() a font in the custom_font_cache

	//! The screen parameter is the screen this fontcache is a part of.
	//!
	//! Creates or returns a current_fontcollection for font_spec of
	//! the given depth.

	current_fontcollection create_custom_font(const screen &s,
						  depth_t depth,
						  const font &font_spec);

	//! Theme fonts are cached by their font name+alpha depth
	typedef std::pair<std::string, depth_t> theme_font_cache_key_t;

	//! Theme font cache container.
	typedef x::weakunordered_multimap<theme_font_cache_key_t,
					  current_fontcollectionObj>
		theme_font_cache_t;

	//! Theme fonts.
	const theme_font_cache_t theme_font_cache;

	//! find_or_create() a font in the theme_font_cache

	//! The screen parameter is the screen this fontcache is a part of.
	//!
	//! Creates or returns a current_fontcollection for font_spec of
	//! the given depth.

	current_fontcollection create_theme_font(const screen &s,
						 depth_t depth,
						 const std::string &theme_font);
	//! Constructor
	screen_fontcachesObj();

	//! Destructor
	~screen_fontcachesObj();
};

LIBCXXW_NAMESPACE_END

#endif
