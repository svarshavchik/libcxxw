/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef freetypefontobj_H
#define freetypefontobj_H

#include <x/obj.H>
#include <x/functional.H>
#include <courier-unicode.h>
#include "fonts/freetypefontfwd.H"
#include "fonts/composite_text_streamfwd.H"
#include "x/w/gcfwd.H"
#include "x/w/screenfwd.H"
#include "x/w/types.H"

LIBCXXW_NAMESPACE_START

//! An actual discrete \ref freetypefont "font opened via the freetype library".

//! \internal

class LIBCXX_HIDDEN freetypefontObj : virtual public obj {

public:

	//! Font ascender above baseline
	const dim_t ascender;

	//! Font descender below baseline
	const dim_t descender;

	//! Font max advance
	const dim_t max_advance;

	//! Font nominal width

	//! This is the same as max_advance for fixed_width fonts. For
	//! proportional font this is determined based on heuristics.
	const dim_t nominal_width;

	//! Whether this is a fixed width font
	const bool fixed_width;

	//! Font's height
	inline dim_t height() const { return dim_t::truncate(ascender+descender); }

	class implObj;

	//! Internal implementation details

	const ref<implObj> impl;

	//! Constructor
	freetypefontObj(dim_t ascender,
			dim_t descender,
			dim_t max_advance,
			dim_t nominal_width,
			bool fixed_width,
			const ref<implObj> &implArg);

	//! Internal constructor
	freetypefontObj(const freetypefont &original);

	//! Destructor
	~freetypefontObj();

	// Load glyphs into the server, if they have not been loaded yet.

	// The glyphs are defined by an input iterator sequence.

	template<typename iter_type>
		void load_glyphs(iter_type b, iter_type e,
				 char32_t unprintable_char) const
	{
		do_load_glyphs(make_function<bool ()>
			       ([&]
				{
					return b != e;
				}),

			       make_function<char32_t ()>
			       ([&]
				{
					char32_t c=*b;
					++b;
					return c;
				}), unprintable_char);
	}

	//! Add glyphs to a composite_text_stream

	//! The glyphs are defined by an input iterator sequence.
	//!
	//! prev_char is non-0 if we're appending to a composite_text_stream
	//! and this is the last character from the previous chunk (for kerning
	//! purposes).
	//!
	//! Updates the virtual rendering position with x and y.
	//!
	//! unprintable_char sets the replacement character for unprintable
	//! characters (characters U+0x0000 to U+0x001F). A 0 character
	//! does not add any glyph. richtexteditorwindow passes a space, in
	//! order to draw a space instead of a newline.

	template<typename iter_type>
		void glyphs_to_stream(iter_type b,
				      iter_type e,
				      composite_text_stream &s,
				      coord_t &x,
				      coord_t &y,
				      char32_t prev_char,
				      char32_t unprintable_char) const
	{
		do_glyphs_to_stream(make_function<bool ()>
				    ([&]
				     {
					     return b != e;
				     }),

				    make_function<char32_t ()>
				    ([&]
				     {
					     char32_t c=*b;
					     ++b;
					     return c;
				     }),
				    s, x, y, prev_char,
				    unprintable_char);
	}

	//! Compute width of unicode characters.

	//! Obtain widths of each unicode character in an input iterator
	//! sequence. The lambda gets invoked for each unicode character
	//! in the input sequence, receiving two pieces of information: the
	//! character's width, in pixels, and its kerning offset from the
	//! previous character.
	//!
	//! If the lambda returns true, glyphs_width() proceeds
	//! with the next unicode character. If the lambda returns false,
	//! glyphs_width() returns.
	//!
	//! unprintable_char sets the replacement character for unprintable
	//! characters (characters U+0x0000 to U+0x001F). A 0 character
	//! returns a 0 width. richtexteditorwindow passes a space, in order
	//! to make the cursor visible on paragraph newline characters.

	template<typename iter_type, typename lambda_type>
		void glyphs_width(iter_type b,
				  iter_type e,
				  lambda_type &&lambda,
				  char32_t prev_char,
				  char32_t unprintable_char) const
	{
		return do_glyphs_width(make_function<bool ()>
				       ([&]
					{
						return b != e;
					}),

				       make_function<char32_t ()>
				       ([&]
					{
						char32_t c=*b;
						++b;
						return c;
					}),
				       make_function<bool(dim_t, int16_t)>
				       ([&]
					(dim_t w, int16_t kerning)
					{
						return lambda(w, kerning);
					}),
				       prev_char, unprintable_char);
	}

 private:

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
