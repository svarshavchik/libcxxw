/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_impl_composite_text_stream_H
#define x_w_impl_composite_text_stream_H

#include <xcb/xcb_renderutil.h>
#include <x/w/impl/fonts/freetypefontfwd.H>
#include <x/w/namespace.H>
#include <x/w/types.H>
#include <x/w/picturefwd.H>
#include <x/w/impl/fonts/composite_text_streamfwd.H>

LIBCXXW_NAMESPACE_START

//! Output of \ref freetypefont "font's glyphs_to_stream()".

//! This is passed, as a parameter, to glyphs_to_stream(), which places the
//! text into the xcb render library's composite_text_stream object, for
//! rendering.
//!
//! The constructor takes a reference to the first freetypefont object
//! whose glyphs_to_stream() method is about to be called. The 2nd parameter
//! is an estimate of the number of characters, or glyphs, that will be
//! rendered, and an estimate of any additional fonts whose glyphs_to_stream()
//! methods will be used with this composite_text_stream object.
//!
//! After one or more glyphs_to_stream()s initialize this object, composite()
//! composes the glyphs into a picture.
//!
//! This is not a thread-safe object.

class composite_text_stream {

public:

	//! Allocated by the constructor
	xcb_render_util_composite_text_stream_t * const s;

	//! First glyph being rendered.
	bool first_glyph=true;

	//! Constructor.

	composite_text_stream(const const_freetypefont &font,
			      uint32_t  total_glyphs,
			      uint32_t  total_glyphset_changes );

	//! Destructor
	~composite_text_stream();

	//! This object cannot be copied.
	composite_text_stream(const composite_text_stream &)=delete;

	//! This object cannot be copied.
	composite_text_stream &operator=(const composite_text_stream &)=delete;

	//! Render the composite text stream

	void composite(//! The composite text stream gets rendered into this picture
		       const picture &dst,

		       //! The color that gets composited into dst.
		       const const_picture &color,

		       //! Starting coordinate of color that gets composited.
		       coord_t color_x,

		       //! Starting coordinate of color that gets composited.
		       coord_t color_y) const;
};

LIBCXXW_NAMESPACE_END

#endif
