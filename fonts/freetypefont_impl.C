/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "fonts/freetypefont_impl.H"
#include "messages.H"
#include "fonts/fontconfig.H"
#include "fonts/composite_text_stream.H"
#include <x/messages.H>
#include <x/logger.H>

#include <fontconfig/fontconfig.h>
#include <fontconfig/fcfreetype.h>

#include FT_BITMAP_H

#define UNPRINTABLE(c) ((c) < 32)

LOG_CLASS_INIT(LIBCXX_NAMESPACE::w::freetypefontObj::implObj);

LIBCXXW_NAMESPACE_START

static inline FT_Face create_face(const const_freetype &library,
				  const std::string &filename,
				  int font_index,
				  dim_t width,
				  dim_t height,
				  dim_t &ascender_value,
				  dim_t &descender_value,
				  dim_t &max_advance)
{
	FT_Face f;

	freetypeObj::library_t::lock lock(library->library);

	auto error=FT_New_Face(*lock, filename.c_str(),
			       font_index,
			       &f);

	if (error == FT_Err_Unknown_File_Format)
		throw EXCEPTION((std::string)
				gettextmsg(_("%1%: unsupported font format"),
					   filename));

	if (error)
		throw EXCEPTION((std::string)
				gettextmsg(_("%1%: cannot open font"),
					   filename));

	// Opening the font was the easy part, let's try to set its size to
	// what we want.

	if (FT_Set_Pixel_Sizes(f,
			       dim_t::value_type(width),
			       dim_t::value_type(height)))
	{
		FT_Done_Face(f);

		throw EXCEPTION("Unable to set font size of "
				<< filename << " to " << width << "x"
				<< height);
	}

	// Capture font ascender and descender metrics.

	ascender_value=(f->size->metrics.ascender+63) >> 6;
	auto d=f->size->metrics.descender;
	descender_value=d > 0 ? 0:(-d + 63) >> 6;
	max_advance=f->size->metrics.max_advance >> 6;

	return f;
}

freetypefontObj::implObj::implObj(const const_screen &screenArg,
				  const const_freetype &libraryArg,
				  const font_id_t &font_idArg,
				  const std::string &filename,
				  int font_index,
				  dim_t width,
				  dim_t height,
				  dim_t &ascender_value,
				  dim_t &descender_value,
				  dim_t &max_advance)
	: glyphset(ref<glyphsetObj>
		   ::create(screenArg,
			    screenArg->find_alpha_pictformat_by_depth
			    (font_idArg.depth))),
	  face(libraryArg, create_face(libraryArg, filename, font_index,
				       width, height, ascender_value,
				       descender_value,
				       max_advance)),
	  has_kerning(FT_HAS_KERNING((*face_t::lock(face)))),
	  depth(font_idArg.depth)
{
}

freetypefontObj::implObj::~implObj()
{
	face_t::lock lock(face);

	FT_Done_Face(*lock);
}

// Load glyphs from a freetype font into the display server. We look up
// the unicode character in the font. We check if the character has been
// loaded, if not we load it into the server.

void freetypefontObj::implObj
::do_load_glyphs(const function<bool ()> &more,
		 const function<char32_t ()>&next,
		 char32_t unprintable_char) const
{
	face_t::const_lock lock(face);

	// Estimate the number of bytes per glyph based on font metrics.

	uint16_t h=((*lock)->size->metrics.height+63) >> 6;
	uint16_t w=((*lock)->size->metrics.max_advance+63) >> 6;

	auto add=glyphset->add_glyphs(w, h);

	// Graylevel scaling.

	const auto num_alpha=
		(((uint32_t)1) << depth_t::value_type(depth))-1;

	while (more())
	{
		// Look up the character in the font
		auto c=next();

		if (UNPRINTABLE(c))
		{
			c=unprintable_char;

			if (UNPRINTABLE(c))
				continue;
		}

		auto glyph_index=FT_Get_Char_Index((*lock), c);

		if (!add->ready_to_add_glyph(glyph_index))
			continue;

		if (FT_Load_Glyph((*lock), glyph_index, FT_LOAD_RENDER))
		{
			LOG_ERROR("FT_Load_Glyph failed for glyph " << c);
			continue;
		}

		if (FT_Render_Glyph((*lock)->glyph,
				    FT_RENDER_MODE_NORMAL))
		{
			LOG_ERROR("FT_Render_Glyph failed for glyph " << c);
		}

		xcb_render_glyphinfo_t glyphinfo={
			.width=(uint16_t)(*lock)->glyph->bitmap.width,
			.height=(uint16_t)(*lock)->glyph->bitmap.rows,
			.x=(int16_t)-(*lock)->glyph->bitmap_left,
			.y=(int16_t)(*lock)->glyph->bitmap_top,
			.x_off=(int16_t)((*lock)->glyph->advance.x >> 6),
			.y_off=(int16_t)((*lock)->glyph->advance.y >> 6),
		};

		freetypeObj::ftbitmap bitmap(face.get_library());

		// Thank goodness for recursive mutexes

		freetypeObj::ftbitmap::bitmap_t::lock bmlock(bitmap.bitmap);

		{
			freetypeObj::library_t::lock
				library_lock(bitmap.bitmap.get_library()
					     ->library);

			// Convert to grayscale.

			if (FT_Bitmap_Convert(*library_lock,
					      &(*lock)->glyph->bitmap,
					      &*bmlock,
					      sizeof(int)))
			{
				LOG_ERROR("FT_Bitmap_Convert failed for glyph "
					  << c);
				continue;
			}
		}

		if (bmlock->pixel_mode != FT_PIXEL_MODE_GRAY)
		{
			LOG_ERROR("Unknown freetype bitmap format "
				  << (int)bmlock->pixel_mode);
			continue;
		}

		// Handle bitmap data

		auto bmbuffer=bmlock->buffer;

		// Number of gray levels in the font.

		auto num_grays=bmlock->num_grays;

		if (num_grays < 2)
		{
			LOG_ERROR(num_grays << " gray levels for glyph " << c);
			continue;
		}

		add->add_glyph(glyph_index, glyphinfo,
			       [&]
			       (size_t y)
			       {
				       auto p=bmbuffer;

				       bmbuffer += bmlock->pitch;

				       return [p, num_alpha, num_grays]
					       (size_t i)
				       {
					       // Scale gray level
					       // to alpha depth range
					       return ((uint16_t)p[i]
						       * num_alpha
						       / (num_grays-1));
				       };
			       });
	}
}

//
// do_glyphs_to_stream() and do_glyphs_width() use the same algorithm to
// process each individual glyph's metrics.
//

void freetypefontObj::implObj
::do_glyphs_to_stream(const function<bool ()> &more,
		      const function<char32_t ()> &next,
		      composite_text_stream &s,
		      coord_t &x,
		      coord_t &y,
		      char32_t prev_char,
		      char32_t unprintable_char)
	const
{
	face_t::const_lock lock(face);

	glyphsetObj::get_loaded_glyphs glyphs(*glyphset);

	auto prev_glyph=prev_char ? FT_Get_Char_Index((*lock), prev_char):0;

	coord_t initial_x=x;
	coord_t initial_y=y;

	while (more())
	{
		// Look up the character in the font
		auto c=next();

		if (UNPRINTABLE(c))
		{
			c=unprintable_char;

			if (UNPRINTABLE(c))
			{
				prev_glyph=0;
				continue;
			}
		}

		uint32_t glyph_index=FT_Get_Char_Index((*lock), c);

		auto iter=glyphs->find(glyph_index);

		if (iter == glyphs->end())
		{
			LOG_ERROR((std::string)
				  gettextmsg(_("No glyph information found for character %1%"),
					     c));
			prev_glyph=0;
			continue;
		}

		int16_t delta_x=0;
		int16_t delta_y=0;

		if (has_kerning && prev_glyph && glyph_index)
		{
			FT_Vector delta;

			FT_Get_Kerning( (*lock), prev_glyph, glyph_index,
					FT_KERNING_DEFAULT, &delta );

			delta_x += (delta.x >> 6);
			delta_y += (delta.y >> 6);
		}

		xcb_render_util_glyphs_32(s.s,
					  coord_t::value_type(initial_x + delta_x),
					  coord_t::value_type(initial_y + delta_y), 1, &glyph_index);

		initial_x=0;
		initial_y=0;

		x += iter->second.x_off;
		y += iter->second.y_off;

		x += delta_x;
		y += delta_y;

		prev_glyph=glyph_index;
	}
}

void freetypefontObj::implObj::do_glyphs_width(const function<bool ()> &more,
					 const function<char32_t ()> &next,
					 const function<bool(dim_t, int16_t)> &width,
					 char32_t prev_char,
					 char32_t unprintable_char)
	const
{
	face_t::const_lock lock(face);

	glyphsetObj::get_loaded_glyphs glyphs(*glyphset);

	auto prev_glyph=prev_char ? FT_Get_Char_Index((*lock), prev_char):0;

	while (more())
	{
		// Look up the character in the font
		auto c=next();

		if (UNPRINTABLE(c))
		{
			c=unprintable_char;

			if (UNPRINTABLE(c))
			{
				prev_glyph=0;
				if (!width(0, 0))
					return;

				continue;
			}
		}

		uint32_t glyph_index=FT_Get_Char_Index((*lock), c);

		auto iter=glyphs->find(glyph_index);

		if (iter == glyphs->end())
		{
			LOG_ERROR((std::string)
				  gettextmsg(_("No glyph information found for character %1%"),
					     c));
			prev_glyph=0;
			if (!width(0, 0))
				return;

			continue;
		}

		int16_t kerning=0;

		if (has_kerning && prev_glyph && glyph_index)
		{
			FT_Vector delta;

			FT_Get_Kerning( (*lock), prev_glyph, glyph_index,
					FT_KERNING_DEFAULT, &delta );

			kerning=(delta.x >> 6);
		}
		if (!width(iter->second.x_off, kerning))
			return;
		prev_glyph=glyph_index;
	}
}

LIBCXXW_NAMESPACE_END
