/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef fontcollectionobj_H
#define fontcollectionobj_H

#include <x/ref.H>
#include <x/obj.H>
#include "x/w/screenfwd.H"
#include "fonts/fontlistobj.H"
#include "fonts/freetypefont.H"
#include "fonts/fontcharsetfwd.H"

#include "x/w/types.H"

LIBCXXW_NAMESPACE_START

//! A collection of fonts.

//! \see fontcollection

class LIBCXX_HIDDEN fontcollectionObj : virtual public obj {

public:
	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	fontcollectionObj(const ref<implObj> &implArg);

	//! Destructor
	~fontcollectionObj();

	//! Fonts' largest ascender
	dim_t ascender() const;

	//! Fonts' largest descender
	dim_t descender() const;

	//! Fonts' height
	dim_t height() const;

	//! Font's maximum advance
	dim_t max_advance() const;

	//! Font's nominal width

	//! This is the same as max_advance() for a fixed_width() font,
	//! otherwise this is a heuristically-computed estimate.
	dim_t nominal_width() const;

	//! Whether this is a fixed width font
	bool fixed_width() const;

	//! Return the best matching font's charset
	fontconfig::const_charset default_charset() const;

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
		void lookup(iter_type beg_iter,
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
				   if (!prev_font.null())
					   callback(prev_value, cur_iter,
						    prev_font);
				   prev_value=cur_iter;

				   prev_font=font;
			   }));

		if (!prev_font.null())
			callback(prev_value, beg_iter,
				 prev_font);
	}

	//! Load the glyphs for the given unicode character sequence.

	//! Uses lookup() and invoke load_glyphs() for each font that covers
	//! the specified text string.

	template<typename iter_type>
		void load_glyphs(const iter_type &beg_iter,
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
		void glyphs_width(iter_type &&beg_iter,
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

 private:

	//! type-erased lookup() implementation.

	void do_lookup(const function< bool(char32_t &) > &next,
		       const function< void(const freetypefont &)
		       > &callback);

};


LIBCXXW_NAMESPACE_END

#endif
