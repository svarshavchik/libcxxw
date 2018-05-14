/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "x/w/screen.H"
#include "fonts/fontpattern.H"
#include "fonts/fontcharset.H"
#include "fonts/freetypefont_impl.H"
#include "fonts/freetype.H"
#include "messages.H"
#include "fonts/cached_font.H"
#include "fonts/fontid_t_hash.H"
#include "connection.H"

#include <x/singleton.H>
#include <x/exception.H>
#include <x/logger.H>
#include FT_BITMAP_H
#include <fontconfig/fontconfig.h>

LOG_CLASS_INIT(LIBCXX_NAMESPACE::w::freetypeObj);

LIBCXXW_NAMESPACE_START

// Singleton constructor

freetypeObj::freetypeObj()
{
	library_t::lock lock(library);

	if (FT_Init_FreeType(&*lock))
		throw EXCEPTION("Unable to initialize the freetype library");
}

freetypeObj::~freetypeObj()
{
	library_t::lock lock(library);
	FT_Done_FreeType(*lock);
}

/////////////////////////////////////////////////////////////////////////////

#if 0
class LIBCXX_HIDDEN cached_freetypefontObj : public freetypefontObj {

 public:
	const cached_font orig;

	cached_freetypefontObj(const cached_font &origArg)
		: freetypefontObj(origArg->f), orig(origArg)
	{
	}

	~cached_freetypefontObj()
	{
	}
};

freetypefontptr
freetypeObj::implObj::font(const const_drawable &drawable,
			   const fontconfig::const_pattern &pattern)
	const
{
	auto drawable_impl=drawable->impl;

	auto cachedfont=font(drawable_impl->get_screen(),
			     drawable_impl->font_alpha_depth(),
			     pattern);

	if (cachedfont.null())
		return freetypefontptr();

	return ref<cached_freetypefontObj>::create(cachedfont);
}
#endif

cached_fontptr freetypeObj::font(const const_screen &screenArg,
				depth_t depth,
				const fontconfig::const_pattern &pattern) const
{
	// Unparse the pattern, use it as the font's
	// key in the global font cache.
	std::string font_key=pattern->unparse();

	std::string filename;
	int index;
	double pixel_size;
	fontconfig::const_charsetptr cs;

	if (!pattern->get_charset(FC_CHARSET, cs))
		throw EXCEPTION(_("Invalid font pattern (FC_CHARSET value is missing)"));

	if (!pattern->get_string(FC_FILE, filename))
		throw EXCEPTION(_("Invalid font pattern (FC_FILE value is missing)"));
	if (!pattern->get_integer(FC_INDEX, index))
		index=0;

	if (!pattern->get_double(FC_PIXEL_SIZE,
				 pixel_size))
		throw EXCEPTION(_("Invalid font pattern(FC_PIXEL_SIZE value is missing"));

	font_id_t id(font_key, depth);

	// Perhaps this font has already been created.

	mpobj<font_cache_t>::lock
		lock(screenArg->get_connection()->impl->font_cache);

	cached_fontptr f;

	try {
		f=(*lock)->find_or_create
			(id,
			 [&, this]
			 {
				 dim_t ascender_value;
				 dim_t descender_value;
				 dim_t max_advance;
				 dim_t height=(dim_t::value_type)
					 std::round(pixel_size);
				 bool fixed_width;

				 auto impl=ref<freetypefontObj::implObj>
					 ::create(screenArg,
						  const_freetype(this), id,
						  filename,
						  index,
						  0, height,
						  ascender_value,
						  descender_value,
						  max_advance, fixed_width);

				 dim_t nominal_width=max_advance;

				 if (!fixed_width)
				 {
					 // Calculate nominal font width by calculating
					 // the average width of the first page's
					 // worth of characters in the font. We start
					 // by finding a page with at least 32
					 // defined characters.

					 std::vector<char32_t> characters_in_font;
					 cs->enumerate
						 ([&]
						  (const auto &coverage)
						  {
							  if (coverage.size() < 32)
								  return true;

							  characters_in_font=coverage;
							  return false;
						  });

					 // Now, get the width info.

					 dim_squared_t total_width=0;

					 for (auto c:characters_in_font)
						 total_width +=
							 impl->width_lookup(c);

					 if (!characters_in_font.empty())
						 nominal_width =
							 dim_t::truncate
							 (total_width /
							  characters_in_font
							  .size());

					 if (nominal_width < 2)
						 nominal_width=1;
				 }

				 auto ftf=freetypefont::create(ascender_value,
							       descender_value,
							       max_advance,
							       nominal_width,
							       fixed_width,
							       impl);

				 return cached_font::create(ftf, cs);
			 });
	} catch (const exception &e)
	{
		e->caught();
	}

	return f;
}


freetypeObj::ftbitmap::ftbitmap(const const_freetype &libraryArg)
	: bitmap(libraryArg)
{
	bitmap_t::lock lock(bitmap);

	FT_Bitmap_Init(&*lock);
}

freetypeObj::ftbitmap::~ftbitmap()
{
	bitmap_t::lock lock(bitmap);

	FT_Bitmap_Done(lock.library(), &*lock);
}

static singleton<freetypeObj> freetype_library;

freetype freetypeBase::create()
{
	return freetype_library.get();
}

LIBCXXW_NAMESPACE_END
