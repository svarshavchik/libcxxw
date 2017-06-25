/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "screen.H"
#include "screen_fontcaches.H"
#include "generic_window_handler.H"
#include "x/w/font_hash.H"
#include "fonts/current_fontcollection.H"
#include "x/number_hash.H"
#include "defaulttheme.H"
#include <x/weakunordered_multimap.H>

namespace std {

	//! Specialization of \c std::hash for custom font cache key.

	template<>
	struct LIBCXX_HIDDEN hash<pair<LIBCXX_NAMESPACE::w::font,
				       LIBCXX_NAMESPACE::w::depth_t>>
		: public hash<LIBCXX_NAMESPACE::w::font>,
		hash<LIBCXX_NAMESPACE::w::depth_t> {

		inline size_t operator()(const auto &key) const noexcept
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

		inline size_t operator()(const auto &key) const noexcept
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
generic_windowObj::handlerObj::create_font(const font &font_spec)
{
	auto s=get_screen();

	return s->impl->fontcaches->create_custom_font(s, font_alpha_depth(),
						       font_spec);
}

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

// Subclass of current_fontcollectionObj that implements a real theme font.

class LIBCXX_HIDDEN current_themefontcollectionObj
	: public current_fontcollectionObj {

	std::string font_name;

 public:
	current_themefontcollectionObj(const screen &font_screen,
				       depth_t depth,
				       const std::string &font_name)
		: current_themefontcollectionObj
		(font_screen,
		 depth,
		 font_name,
		 *current_theme_t::lock{
			font_screen->impl->current_theme
				})
	{
	}

	current_themefontcollectionObj(const screen &font_screen,
				       depth_t depth,
				       const std::string &font_name,
				       const defaulttheme &font_theme)
		: current_fontcollectionObj(font_screen, depth,
					    font_theme->get_theme_font
					    (font_name),
					    font_theme),
		font_name(font_name)
		{
		}

 private:

	// Update the font specs.

	void theme_was_really_updated(IN_THREAD_ONLY) override
	{
		font_spec(IN_THREAD)=
			font_theme->get_theme_font(font_name);
	}
};

current_fontcollection generic_windowObj::handlerObj
::create_theme_font(const std::experimental::string_view &font_name_sv)
{
	// TODO: gcc 6.3.1, string_view support is incomplete.
	std::string font_name{font_name_sv.begin(), font_name_sv.end()};

	auto s=get_screen();
	auto depth=font_alpha_depth();

	return s->impl->fontcaches->theme_font_cache
		->find_or_create
		({font_name, depth},
		 [&]
		 {
			 return ref<current_themefontcollectionObj>
				 ::create(s, depth, font_name);
		 });
}


LIBCXXW_NAMESPACE_END
