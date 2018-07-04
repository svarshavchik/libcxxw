/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef freetypefont_impl_H
#define freetypefont_impl_H

#include <x/ref.H>
#include <x/logger.H>
#include <x/functional.H>
#include <courier-unicode.h>

#include "x/w/impl/fonts/freetypefont.H"
#include "x/w/impl/fonts/fontcharsetfwd.H"
#include "fonts/fontconfigfwd.H"

#include "fonts/freetype.H"
#include "fonts/fontid_t.H"
#include "xid_t.H"
#include "fonts/glyphset.H"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <xcb/xcb.h>
#include <xcb/render.h>
#include <vector>
#include <set>
#include <map>

LIBCXXW_NAMESPACE_START

//! \ref freetypefont "freetypefont" implementation object.

class LIBCXX_HIDDEN freetypefontObj::implObj : virtual public obj {

	LOG_CLASS_SCOPE;

	//! The underlying glyphset under perpetual construction.
	const ref<glyphsetObj> glyphset;

	//! The thread-safe container for the underlying FT_Face
	typedef freetypeObj::ft_object<FT_Face> face_t;

	//! The freetype face.
	face_t face;

	//! Whether the font provides kerning.
	const bool has_kerning;

	//! Cached depth of the undertlying bitmap

	const depth_t depth;

 public:

	//! Constructor

	implObj(const const_screen &screenArg,
		const const_freetype &libraryArg,
		const font_id_t &font_idArg,
		const std::string &filename,
		int font_index,

		dim_t width,
		dim_t height,
		dim_t &ascender_value,
		dim_t &descender_value,
		dim_t &max_advance,
		bool &fixed_width);

	~implObj();

	friend class composite_text_stream;

	//! Type-erased load_glyphs() implementation.

	void do_load_glyphs(const function<bool ()> &more,
			    const function<char32_t ()> &next,
			    char32_t unprintable_char) const;

	//! Type-erased glyphs_to_stream() implementation.

	void do_glyphs_to_stream(const function<bool ()> &more,
				 const function<char32_t ()> &next,
				 composite_text_stream &s,
				 coord_t &x,
				 coord_t &y,
				 char32_t prev_char,
				 char32_t unprintable_char) const;

	//! Type-erased glyphs_width() implementation.

	void do_glyphs_width(const function<bool ()> &more,
			     const function<char32_t ()> &next,
			     const function<bool(dim_t, int16_t)> &width,
			     char32_t prev_char,
			     char32_t unprintable_char) const;

	//! Quick lookup of a character's pixel width.

	//! Used only when loading a font, to compute its nominal width.

	dim_t width_lookup(char32_t c);

 private:

	//! Call load_and_render_glyph(), and if everything is cool, add it.

	bool add_real_glyph(face_t::const_lock &lock,
			    const size_t glyph_index,
			    const char32_t c,
			    const ref<glyphsetObj::addObj> &add,
			    const uint32_t num_alpha) const;

	//! Prepare a glyph for rendering.

	//! \internal
	static bool load_and_render_glyph(face_t::const_lock &lock,
					  size_t glyph_index, char32_t c);
};

LIBCXXW_NAMESPACE_END

#endif
