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

#include "fonts/freetypefont.H"
#include "fonts/fontcharsetfwd.H"
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
		dim_t &max_advance);

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
};

LIBCXXW_NAMESPACE_END

#endif
