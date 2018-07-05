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
//! The 2nd and subsequent invocations of callback() receive a beginning
//! iterator value that was the ending iterator value of the previous
//! invocation.

template<typename iter_type, typename callback_functor>
void fontcollectionObj::lookup(iter_type beg_iter,
			       const iter_type &end_iter,
			       callback_functor &&callback,
			       char32_t unprintable_char)
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
			 }),
			unprintable_char);

	if (prev_font)
		callback(prev_value, beg_iter,
			 prev_font);
}

//! Load the glyphs for the given unicode character sequence.

//! load_glyphs() must be used before rendering text, to load each unicode
//! character's glyphs. Nothing happens if the glyph is already loaded.
//!
//! Uses lookup() and invokes load_glyphs() for each font that covers
//! the specified text string.

template<typename iter_type>
void fontcollectionObj::load_glyphs(const iter_type &beg_iter,
				    const iter_type &end_iter,
				    char32_t unprintable_char)
{
	lookup(beg_iter, end_iter,
	       [unprintable_char]
	       (const iter_type &beg_iter,
		const iter_type &end_iter,
		const freetypefont &f)
	       {
		       f->load_glyphs(beg_iter, end_iter,
				      unprintable_char);
	       },
	       unprintable_char);
}

//! Calculate unicode text width.

//! Uses lookup() and invokes glyphs_size_and_kernings() of each underlying
//! \ref freetypefont font.

template<typename iter_type, typename lambda_type>
void fontcollectionObj::glyphs_size_and_kernings(iter_type &&beg_iter,
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
		       f->glyphs_size_and_kernings(beg_iter, end_iter,
						   lambda, prev_char,
						   unprintable_char);
	       },
	       unprintable_char);
}

LIBCXXW_NAMESPACE_END

#endif
