/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "fonts/freetypefont_impl.H"

LIBCXXW_NAMESPACE_START

freetypefontObj::freetypefontObj(dim_t ascender,
				 dim_t descender,
				 dim_t max_advance,
				 dim_t nominal_width,
				 bool fixed_width,
				 const ref<implObj> &impl)
	: ascender(ascender), descender(descender),
	  max_advance(max_advance),
	  nominal_width(nominal_width), fixed_width(fixed_width),
	  impl(impl)
{
}

freetypefontObj::freetypefontObj(const freetypefont &original)
	: ascender(original->ascender),
	  descender(original->descender),
	  max_advance(original->max_advance),
	  fixed_width(original->fixed_width),
	  impl(original->impl)
{
}

freetypefontObj::~freetypefontObj()
{
}

void freetypefontObj::do_load_glyphs(const function<bool ()> &more,
				     const function<char32_t ()>&next,
				     char32_t unprintable_char)
	const
{
	impl->do_load_glyphs(more, next, unprintable_char);
}

void freetypefontObj::do_glyphs_to_stream(const function<bool ()> &more,
					  const function<char32_t ()> &next,
					  composite_text_stream &s,
					  coord_t &x,
					  coord_t &y,
					  char32_t prev_char,
					  char32_t unprintable_char) const
{
	impl->do_glyphs_to_stream(more, next, s, x, y, prev_char,
				  unprintable_char);
}

void freetypefontObj
::do_glyphs_size_and_kernings(const function<bool ()> &more,
			      const function<char32_t ()> &next,
			      const function<bool(dim_t, dim_t,
						  int16_t, int16_t)> &callback,
			      char32_t prev_char,
			      char32_t unprintable_char) const
{
	impl->do_glyphs_size_and_kernings(more, next, callback,
					  prev_char, unprintable_char);
}

LIBCXXW_NAMESPACE_END
