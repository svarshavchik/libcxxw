/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef screen_fontcaches_h
#define screen_fontcaches_h

#include "screen_fontcachesfwd.H"
#include "x/w/fontfwd.H"
#include "x/w/types.H"
#include "x/w/current_fontcollectionfwd.H"
#include <x/weakunordered_multimapfwd.H>
#include <x/obj.H>
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

//! A cache of \ref current_fontcollection "current font collections" for this screen.

class LIBCXX_HIDDEN screen_fontcachesObj : virtual public obj {

 public:

	//! Custom fonts are cached by their font description+alpha depth.

	typedef x::weakunordered_multimap<std::pair<font, depth_t>,
					  current_fontcollectionObj>
		custom_font_cache_t;

	//! Custom fonts.
	const custom_font_cache_t custom_font_cache;

	//! Theme fonts are cached by their font name+alpha depth
	typedef x::weakunordered_multimap<std::pair<std::string, depth_t>,
					  current_fontcollectionObj>
		theme_font_cache_t;

	//! Custom fonts.
	const theme_font_cache_t theme_font_cache;

	//! Constructor
	screen_fontcachesObj();

	//! Destructor
	~screen_fontcachesObj();
};

LIBCXXW_NAMESPACE_END

#endif
