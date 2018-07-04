/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef composite_text_stream_H
#define composite_text_stream_H

#include <xcb/xcb_renderutil.h>
#include "fonts/freetypefontfwd.H"
#include "x/w/namespace.H"
#include "x/w/types.H"
#include "x/w/picturefwd.H"

LIBCXXW_NAMESPACE_START

//! Output of \ref freetypefont "font's glyphs_to_stream()".

//! This is passed, as a parameter, to glyphs_to_stream(), which places the
//! text into the xcb render library's composite_text_stream object, for
//! rendering.

class LIBCXX_HIDDEN composite_text_stream {

public:

	//! Allocated by the constructor
	xcb_render_util_composite_text_stream_t * const s;

	//! Constructor.

	composite_text_stream(const const_freetypefont &font,
			      uint32_t  total_glyphs,
			      uint32_t  total_glyphset_changes );

	//! Destructor
	~composite_text_stream();

	composite_text_stream(const composite_text_stream &)=delete;
	composite_text_stream &operator=(const composite_text_stream &)=delete;

	//! Render the composite text stream

	void composite(const picture &dst,
		       const const_picture &color,
		       coord_t color_x,
		       coord_t color_y) const;
};

LIBCXXW_NAMESPACE_END

#endif
