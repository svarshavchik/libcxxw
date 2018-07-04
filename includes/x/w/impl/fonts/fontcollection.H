/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_impl_fonts_fontcollection_H
#define x_w_impl_fonts_fontcollection_H

#include <x/ref.H>
#include <x/functional.H>
#include <x/w/fontcollectionfwd.H>
#include <x/w/impl/fonts/fontcollectionobj.H>
#include <x/w/impl/fonts/freetypefont.H>

LIBCXXW_NAMESPACE_START

//! Look up fonts for a unicode character sequence.

//! Find fonts for each unicode character in some sequence of unicode
//! characters. The sequence's beginning and ending iterators must be
//! at minimum, forward iterators. input iterators won't work.
//!
//! The callback() gets invoked with three parameters,
//! the beginning and ending iterator values, and the font for the
//! unicode characters in this sequence.
//!
//! callback() gets invoked at least once if the given sequence of
//! unicode characters is not empty. callback() is guaranteed to be
//! called for the entire original sequence of unicode characters.

template<typename iter_type, typename callback_functor>
void fontcollectionObj::lookup(iter_type beg_iter,
			       const iter_type &end_iter,
			       callback_functor &&callback)
{
	auto prev_value=beg_iter;
	freetypefontptr prev_font;

	iter_type cur_iter;

	this->do_lookup(make_function<bool(char32_t &)>
			([&]
			 (char32_t &c)
			 {
				 if (beg_iter == end_iter)
					 return false;

				 cur_iter=beg_iter;
				 c= *beg_iter;
				 ++beg_iter;
				 return true;
			 }),
			make_function< void(const freetypefont &) >
			([&]
			 (const freetypefont &font)
			 {
				 // cur_iter is the ending iterator value,
				 // the value of beg_iter before it was
				 // incremented.
				 if (prev_font)
					 callback(prev_value, cur_iter,
						  prev_font);
				 prev_value=cur_iter;

				 prev_font=font;
			 }));

	if (prev_font)
		callback(prev_value, beg_iter,
			 prev_font);
}

//! Load the glyphs for the given unicode character sequence.

//! Uses lookup() and invoke load_glyphs() for each font that covers
//! the specified text string.

template<typename iter_type>
void fontcollectionObj::load_glyphs(const iter_type &beg_iter,
				    const iter_type &end_iter)
{
	lookup(beg_iter, end_iter,
	       []
	       (const iter_type &beg_iter,
		const iter_type &end_iter,
		const freetypefont &f)
	       {
		       f->load_glyphs(beg_iter, end_iter);
	       });
}
//! Calculate unicode text width.

//! Use lookup() and invoke glyph_width() of each underlying
//! \ref freetype font.

template<typename iter_type, typename lambda_type>
void fontcollectionObj::glyphs_width(iter_type &&beg_iter,
				     iter_type &&end_iter,
				     lambda_type &&lambda,
				     char32_t prev_char,
				     char32_t unprintable_char)
{
	lookup(beg_iter, end_iter,
	       [&]
	       (const iter_type &beg_iter,
		const iter_type &end_iter,
		const freetypefont &f)
	       {
		       f->glyphs_width(beg_iter, end_iter,
				       lambda, prev_char,
				       unprintable_char);
	       });
}

LIBCXXW_NAMESPACE_END

#endif
