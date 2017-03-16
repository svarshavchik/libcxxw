/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "fonts/composite_text_stream.H"
#include "fonts/freetypefont_impl.H"
#include <x/exception.H>

LIBCXXW_NAMESPACE_START

composite_text_stream
::composite_text_stream(const const_freetypefont &font,
			uint32_t total_glyphs,
			uint32_t total_glyphset_changes )
	: s(xcb_render_util_composite_text_stream(font->impl->glyphset
						  ->glyphset_id(),
						  total_glyphs,
						  total_glyphset_changes
						  ))
{
	if (!s)
		throw EXCEPTION("xcb_render_util_composite_text_stream failed");
}

composite_text_stream::~composite_text_stream()
{
	xcb_render_util_composite_text_free(s);
}

LIBCXXW_NAMESPACE_END
