/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "fonts/freetypefont_impl.H"
#include "messages.H"
#include "fonts/fontconfig.H"
#include "x/w/impl/fonts/composite_text_stream.H"
#include <x/messages.H>
#include <x/logger.H>

#include <fontconfig/fontconfig.h>
#include <fontconfig/fcfreetype.h>
#include <iomanip>

#include FT_BITMAP_H

LOG_CLASS_INIT(LIBCXX_NAMESPACE::w::freetypefontObj::implObj);

LIBCXXW_NAMESPACE_START

static inline FT_Face create_face(const const_freetype &library,
				  const std::string &filename,
				  int font_index,
				  dim_t width,
				  dim_t height,
				  dim_t &ascender_value,
				  dim_t &descender_value,
				  dim_t &max_advance,
				  bool &fixed_width)
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
				gettextmsg(_("%1%: cannot open font: %2%"),
					   filename,
					   freetype_error(error)));

#if 0
	std::cout << "**** OPENED " << filename
		  << " FOR " << width << "x" << height
		  << std::endl;

	for (FT_Int n=0; n<f->num_fixed_sizes; ++n)
		std::cout << "   SIZE: "
			  << f->available_sizes[n].width
			  << "x"
			  << f->available_sizes[n].height
			  << std::endl;
#endif
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
	fixed_width=f->face_flags & FT_FACE_FLAG_FIXED_WIDTH ? true:false;

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
				  dim_t &max_advance,
				  bool &fixed_width)
	: glyphset(ref<glyphsetObj>
		   ::create(screenArg,
			    screenArg->find_alpha_pictformat_by_depth
			    (font_idArg.depth))),
	  face(libraryArg, create_face(libraryArg, filename, font_index,
				       width, height, ascender_value,
				       descender_value,
				       max_advance, fixed_width)),
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
			c=REPLACE_WITH_PRINTABLE(c,unprintable_char);

			if (UNPRINTABLE(c))
				continue;
		}

		auto glyph_index=FT_Get_Char_Index((*lock), c);

		if (!add->ready_to_add_glyph(glyph_index))
			continue;

		if (add_real_glyph(lock, glyph_index, c, add, num_alpha))
			continue;

		// We need a glyph of some kind, no matter what.

		xcb_render_glyphinfo_t glyphinfo={
			.width=1,
			.height=1,
			.x=0,
			.y=0,
			.x_off=0,
			.y_off=0,
		};

		add->add_glyph(glyph_index, glyphinfo,
			       [&]
			       (size_t )
			       {
				       return []
					       (size_t)
				       {
					       return 0;
				       };
			       });
	}
}

bool freetypefontObj::implObj
::add_real_glyph(face_t::const_lock &lock,
		 const size_t glyph_index, const char32_t c,
		 const ref<glyphsetObj::addObj> &add,
		 const uint32_t num_alpha) const
{
	if (!load_and_render_glyph(lock, glyph_index, c))
		return false;

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

		auto error=FT_Bitmap_Convert(*library_lock,
					     &(*lock)->glyph->bitmap,
					     &*bmlock,
					     sizeof(int));

		if (error)
		{
			LOG_ERROR("FT_Bitmap_Convert failed for glyph "
				  << c
				  << ", source mode "
				  << (int)(*lock)->glyph->bitmap.pixel_mode
				  << ", family "
				  << (*lock)->family_name
				  << "/"
				  << (*lock)->style_name
				  << ": "
				  << freetype_error(error));
			return false;
		}
	}

	if (bmlock->pixel_mode != FT_PIXEL_MODE_GRAY)
	{
		LOG_ERROR("Unknown freetype bitmap format "
			  << (int)bmlock->pixel_mode);
		return false;
	}

	// Handle bitmap data

	auto bmbuffer=bmlock->buffer;

	// Number of gray levels in the font.

	auto num_grays=bmlock->num_grays;

	if (num_grays < 2)
	{
		LOG_ERROR(num_grays << " gray levels for glyph " << c);
		return false;
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
	return true;
}

bool freetypefontObj::implObj::load_and_render_glyph(face_t::const_lock &lock,
						     size_t glyph_index,
						     char32_t c)
{
	auto error=FT_Load_Glyph((*lock), glyph_index, FT_LOAD_RENDER);

	if (error)
	{
		error=FT_Load_Glyph((*lock), glyph_index,
				    FT_LOAD_RENDER|FT_LOAD_NO_SCALE);

		if (error)
		{
			LOG_ERROR("FT_Load_Glyph failed for glyph #" << glyph_index
				  << " U+0x"
				  << std::hex << c
				  << ", family "
				  << (*lock)->family_name
				  << "/"
				  << (*lock)->style_name
				  << ": " << freetype_error(error));
			return false;
		}
	}
#if 0
	if (FT_Render_Glyph((*lock)->glyph,
			    FT_RENDER_MODE_NORMAL))
	{
		LOG_ERROR("FT_Render_Glyph failed for glyph " << glyph_index
			  << " U+0x"
			  << std::hex << c
			  FONT_ID);
		return false;
	}
#endif
	return true;
}

dim_t freetypefontObj::implObj::width_lookup(char32_t c)
{
	face_t::const_lock lock(face);

	auto glyph_index=FT_Get_Char_Index((*lock), c);

	dim_t width=0;

	if (load_and_render_glyph(lock, glyph_index, c))
		width=(*lock)->glyph->bitmap.width;

	return width;
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
	xcb_render_util_change_glyphset(s.s, glyphset->glyphset_id());

	coord_t initial_x=x;
	coord_t initial_y=y;

	process_glyphs(more, next, prev_char, unprintable_char,
		       make_function<process_glyphs_callback_t>
		       ([&]
			(uint32_t glyph_index,
			 dim_t w,
			 dim_t h,
			 int16_t kerning_x,
			 int16_t kerning_y)
			{
				if (!glyph_index)
					return true;

				// The first rendered glyph's coordinates are
				// absolute. All others are deltas.

				if (!s.first_glyph)
				{
					initial_x=0;
					initial_y=0;
				}

				xcb_render_util_glyphs_32
					(s.s,
					 coord_t::value_type(initial_x
							     + kerning_x),
					 coord_t::value_type(initial_y
							     + kerning_y),
					 1, &glyph_index);

				s.first_glyph=false;

				x = coord_t::truncate(x+w+kerning_x);
				y = coord_t::truncate(y+h+kerning_y);
				return true;
			}));
}

void freetypefontObj::implObj
::do_glyphs_size_and_kernings(const function<bool ()> &more,
			      const function<char32_t ()> &next,
			      const function<bool(dim_t, dim_t,
						  int16_t, int16_t)
			      > &size_and_kerning,
			      char32_t prev_char,
			      char32_t unprintable_char)
	const
{

	process_glyphs(more, next, prev_char, unprintable_char,
		       make_function<process_glyphs_callback_t>
		       ([&]
			(uint32_t glyph_index,
			 dim_t w,
			 dim_t h,
			 int16_t kerning_x,
			 int16_t kerning_y)
			{
				return size_and_kerning(w, h,
							kerning_x, kerning_y);
			}));
}


void freetypefontObj::implObj
::process_glyphs(const function<bool ()> &more,
		 const function<char32_t ()> &next,
		 char32_t prev_char,
		 char32_t unprintable_char,
		 const function<process_glyphs_callback_t> &callback) const
{
	face_t::const_lock lock(face);

	glyphsetObj::get_loaded_glyphs glyphs(*glyphset);

	if (UNPRINTABLE(prev_char))
	{
		prev_char=REPLACE_WITH_PRINTABLE(prev_char, unprintable_char);

		if (UNPRINTABLE(prev_char))
			prev_char=0;
	}

	auto prev_glyph=prev_char ? FT_Get_Char_Index((*lock), prev_char):0;

	while (more())
	{
		// Look up the character in the font
		auto c=next();

		if (UNPRINTABLE(c))
		{
			c=REPLACE_WITH_PRINTABLE(c,unprintable_char);

			if (UNPRINTABLE(c))
			{
				prev_glyph=0;
				if (!callback(0, 0, 0, 0, 0))
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
					     c)
				  << ", family "
				  << (*lock)->family_name
				  << "/"
				  << (*lock)->style_name);
			prev_glyph=0;
			if (!callback(0, 0, 0, 0, 0))
				return;

			continue;
		}

		int16_t kerning_x=0;
		int16_t kerning_y=0;

		if (has_kerning && prev_glyph)
		{
			FT_Vector delta;

			FT_Get_Kerning( (*lock), prev_glyph, glyph_index,
					FT_KERNING_DEFAULT, &delta );

			kerning_x=(delta.x >> 6);
			kerning_y=(delta.y >> 6);
		}

		if (!callback(glyph_index,
			      iter->second.x_off,
			      iter->second.y_off,
			      kerning_x, kerning_y))
			return;

		prev_glyph=glyph_index;
	}
}
LIBCXXW_NAMESPACE_END
