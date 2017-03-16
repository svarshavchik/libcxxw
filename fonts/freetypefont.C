/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "fonts/freetypefont_impl.H"

LIBCXXW_NAMESPACE_START

freetypefontObj::freetypefontObj(dim_t ascenderArg,
				 dim_t descenderArg,
				 dim_t max_advanceArg,
				 const ref<implObj> &implArg)
	: ascender(ascenderArg), descender(descenderArg),
	  max_advance(max_advanceArg),
	  impl(implArg)
{
}

freetypefontObj::freetypefontObj(const freetypefont &original)
	: ascender(original->ascender),
	  descender(original->descender),
	  max_advance(original->max_advance),
	  impl(original->impl)
{
}

freetypefontObj::~freetypefontObj()
{
}

void freetypefontObj::do_load_glyphs(const function<bool ()> &more,
				     const function<char32_t ()>&next)
	const
{
	impl->do_load_glyphs(more, next);
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

void freetypefontObj::do_glyphs_width(const function<bool ()> &more,
				      const function<char32_t ()> &next,
				      const function<bool(dim_t, int16_t)> &width,
				      char32_t prev_char,
				      char32_t unprintable_char) const
{
	impl->do_glyphs_width(more, next, width, prev_char, unprintable_char);
}

LIBCXXW_NAMESPACE_END
