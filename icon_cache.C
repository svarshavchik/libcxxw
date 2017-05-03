/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "icon_cache.H"
#include "sxg/sxg_image.H"
#include "sxg/sxg_parser.H"
#include "defaulttheme.H"
#include "screen.H"
#include "pixmap.H"
#include "drawable.H"
#include "x/w/picture.H"
#include <x/refptr_hash.H>
#include <x/number_hash.H>
#include <x/weakunordered_multimap.H>
#include <unistd.h>

LIBCXXW_NAMESPACE_START

struct icon_cacheObj::sxg_cache_key_t {

	std::string filename;
	defaulttheme theme;

	//! Comparison operator
	bool operator==(const sxg_cache_key_t &) const;
};


bool icon_cacheObj::sxg_cache_key_t::operator==(const sxg_cache_key_t &o) const
{
	return filename == o.filename && theme == o.theme;
}

struct icon_cacheObj::sxg_cache_key_t_hash
	: public std::hash<std::string>,
	  public std::hash<defaulttheme> {

	size_t operator()(const sxg_cache_key_t &k) const
	{
		return std::hash<std::string>::operator()(k.filename) +
			std::hash<defaulttheme>::operator()(k.theme);
	};
};

// SXG images are cached by the following parameters:

struct icon_cacheObj::sxg_image_cache_key {
	const_pictformat    drawable_pictformat;
	render_repeat       repeat;
	dim_t               width;
	dim_t               height;

	bool operator==(const sxg_image_cache_key &o) const;
};

bool icon_cacheObj::sxg_image_cache_key
::operator==(const sxg_image_cache_key &o) const
{
	return drawable_pictformat == o.drawable_pictformat &&
		repeat == o.repeat &&
		width == o.width &&
		height == o.height;
}

struct icon_cacheObj::sxg_image_cache_key_hash
	: public std::hash<const_pictformat>,
	  public std::hash<dim_t> {

	size_t operator()(const sxg_image_cache_key &k) const
	{
		return std::hash<const_pictformat>::operator()
			(k.drawable_pictformat) ^ (size_t)k.repeat
			^ (std::hash<dim_t>::operator()(k.width) << 16)
			^ (std::hash<dim_t>::operator()(k.height) << 4);
	}
};

icon_cacheObj::icon_cacheObj()
	: sxg_parser_cache{sxg_parser_cache_t::create()},
	  sxg_image_cache(sxg_image_cache_t::create())
{
}

icon_cacheObj::~icon_cacheObj()=default;

sxg_parser get_sxg(const std::experimental::string_view &filename,
		   const screen &screenref,
		   const defaulttheme &theme)
{
	std::string f{filename};

	if (f.find('/') == f.npos && access(f.c_str(), R_OK))
	{
		f=theme->themedir + "/" + f;

		if (filename.find('.') == filename.npos)
			f += ".sxg";
	}
	return screenref->impl->iconcaches->sxg_parser_cache->find_or_create
		({f, theme},
		 [&]
		 {
			 return sxg_parser::create(f, screenref, theme);
		 });
}

LIBCXXW_NAMESPACE_END
