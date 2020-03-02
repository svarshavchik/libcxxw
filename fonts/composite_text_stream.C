/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "x/w/impl/fonts/composite_text_stream.H"
#include "fonts/freetypefont_impl.H"
#include "picture.H"
#include "xid_t.H"
#include "connection_thread.H"
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

void composite_text_stream::composite(const picture &dst,
				      const const_picture &color,
				      coord_t x,
				      coord_t y) const
{
	xcb_render_util_composite_text
		(dst->impl->picture_conn()->conn,
		 XCB_RENDER_PICT_OP_OVER,
		 color->impl->picture_id(),
		 dst->impl->picture_id(),
		 XCB_NONE,
		 coord_t::value_type(x),
		 coord_t::value_type(y),
		 s);
}

LIBCXXW_NAMESPACE_END
