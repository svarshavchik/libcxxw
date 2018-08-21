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

	typedef LIBCXX_NAMESPACE::w::screen_fontcachesObj
	::theme_font_cache_key_t theme_font_cache_key_t;

	typedef LIBCXX_NAMESPACE::w::screen_fontcachesObj
	::custom_font_cache_key_t custom_font_cache_key_t;

	//! Specialization of \c std::hash for custom font cache key.

	template<>
	struct LIBCXX_HIDDEN hash<custom_font_cache_key_t>
		: public hash<LIBCXX_NAMESPACE::w::font>,
		hash<LIBCXX_NAMESPACE::w::depth_t> {

		inline size_t operator()(const custom_font_cache_key_t &key)
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
	struct LIBCXX_HIDDEN hash<theme_font_cache_key_t>
		: public hash<string>,
		hash<LIBCXX_NAMESPACE::w::depth_t> {

		inline size_t operator()(const theme_font_cache_key_t &key)
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
}

current_fontcollection
screen_fontcachesObj::create_custom_font(const screen &s,
					 depth_t depth,
					 const font &font_spec)
{
	// Usually we already have the font.

	custom_font_cache_key_t key{font_spec, depth};

	auto iter=custom_font_cache->find(key);

	if (iter != custom_font_cache->end())
	{
		auto p=iter->second.getptr();

		if (p)
			return p;
	}

	// Acquire a lock before accessing the custom font cache.
	//
	// update_current_theme() also acquires this lock before updating
	// the screen's theme and updating all existing fonts.

	current_theme_t::lock lock{s->impl->current_theme};

	return custom_font_cache
		->find_or_create(key,
				[&]
				{
					return current_fontcollection::create
						(s, depth, font_spec,
						 *lock);
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
				       const std::string &font_name,
				       const defaulttheme &font_theme)
		: current_fontcollectionObj{font_screen, depth,
			font_theme->get_theme_font(font_name),
			font_theme},
		font_name{font_name}
		{
		}

 private:

	// Update the font specs.

	void current_theme_updated(ONLY IN_THREAD,
				   const defaulttheme &font_theme) override
	{
		font_spec(IN_THREAD)=
			font_theme->get_theme_font(font_name);
		current_fontcollectionObj::current_theme_updated(IN_THREAD,
								 font_theme);
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
	// Usually we already have the font.

	theme_font_cache_key_t key{name, depth};

	auto iter=theme_font_cache->find(key);

	if (iter != theme_font_cache->end())
	{
		auto p=iter->second.getptr();

		if (p)
			return p;
	}

	// Acquire a lock before accessing the theme font cache.
	//
	// update_current_theme() also acquires this lock before updating
	// the screen's theme and updating all existing fonts.

	current_theme_t::lock lock{s->impl->current_theme};

	return theme_font_cache->find_or_create
		(key,
		 [&]
		 {
			 return ref<current_themefontcollectionObj>
				 ::create(s, depth, name, *lock);
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
