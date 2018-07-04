/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "screen.H"
#include "screen_fontcaches.H"
#include "generic_window_handler.H"
#include "x/w/font_hash.H"
#include "x/w/impl/fonts/current_fontcollection.H"
#include "x/number_hash.H"
#include "defaulttheme.H"
#include <x/weakunordered_multimap.H>
#include <x/visitor.H>

namespace std {

	//! Specialization of \c std::hash for custom font cache key.

	template<>
	struct LIBCXX_HIDDEN hash<pair<LIBCXX_NAMESPACE::w::font,
				       LIBCXX_NAMESPACE::w::depth_t>>
		: public hash<LIBCXX_NAMESPACE::w::font>,
		hash<LIBCXX_NAMESPACE::w::depth_t> {

		inline size_t operator()(const pair<LIBCXX_NAMESPACE::w::font,
					 LIBCXX_NAMESPACE::w::depth_t> &key)
			const noexcept
		{
			return hash<LIBCXX_NAMESPACE::w::font>::operator()
				(key.first) +
				hash<LIBCXX_NAMESPACE::w::depth_t>::operator()
				(key.second);
		}
	};

	//! Specialization of \c std::hash for theme font cache key.

	template<>
	struct LIBCXX_HIDDEN hash<pair<string,
				       LIBCXX_NAMESPACE::w::depth_t>>
		: public hash<string>,
		hash<LIBCXX_NAMESPACE::w::depth_t> {

		inline size_t operator()(const pair<string,
					 LIBCXX_NAMESPACE::w::depth_t> &key)
			const noexcept
		{
			return hash<string>::operator()(key.first) +
				hash<LIBCXX_NAMESPACE::w::depth_t>::operator()
				(key.second);
		}
	};
}

LIBCXXW_NAMESPACE_START

screen_fontcachesObj::screen_fontcachesObj()
	: custom_font_cache{custom_font_cache_t::create()},
	  theme_font_cache{theme_font_cache_t::create()}
{
}

screen_fontcachesObj::~screen_fontcachesObj()=default;

current_fontcollection
screen_fontcachesObj::create_custom_font(const screen &s,
					 depth_t depth,
					 const font &font_spec)
{
	return custom_font_cache
		->find_or_create({font_spec, depth},
				[&]
				{
					return current_fontcollection::create
						(s, depth, font_spec);
				});
}

namespace {
#if 0
}
#endif

// Subclass of current_fontcollectionObj that implements a real theme font.

class LIBCXX_HIDDEN current_themefontcollectionObj
	: public current_fontcollectionObj {

	std::string font_name;

 public:
	current_themefontcollectionObj(const screen &font_screen,
				       depth_t depth,
				       const std::string &font_name)
		: current_themefontcollectionObj{font_screen,
			depth,
			font_name,
			font_screen->impl->current_theme.get()}
	{
	}

	current_themefontcollectionObj(const screen &font_screen,
				       depth_t depth,
				       const std::string &font_name,
				       const defaulttheme &font_theme)
		: current_fontcollectionObj{font_screen, depth,
			font_theme->get_theme_font(font_name),
			font_theme},
		font_name(font_name)
		{
		}

 private:

	// Update the font specs.

	void theme_was_really_updated(ONLY IN_THREAD) override
	{
		font_spec(IN_THREAD)=
			font_theme->get_theme_font(font_name);
	}
};

#if 0
{
#endif
}

current_fontcollection
screen_fontcachesObj::create_theme_font(const screen &s,
					depth_t depth,
					const std::string &name)
{
	return theme_font_cache->find_or_create
		({name, depth},
		 [&]
		 {
			 return ref<current_themefontcollectionObj>
				 ::create(s, depth, name);
		 });
}

current_fontcollection
elementObj::implObj::create_current_fontcollection(const font_arg &f)
{
	auto &wh=get_window_handler();
	auto s=wh.get_screen();
	auto depth=wh.font_alpha_depth();

	return std::visit
		(visitor{[&](const theme_font &f)
			 {
				 return s->impl->fontcaches
					 ->create_theme_font(s, depth, f.name);
			 },
			 [&](const font &f)
			 {
				 return s->impl->fontcaches
					 ->create_custom_font(s, depth, f);
			 }}, f);
}

LIBCXXW_NAMESPACE_END
