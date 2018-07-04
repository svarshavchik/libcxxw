/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_impl_fonts_fontcollectionobj_H
#define x_w_impl_fonts_fontcollectionobj_H

#include <x/ref.H>
#include <x/obj.H>
#include <x/w/screenfwd.H>
#include <x/w/impl/fonts/freetypefontfwd.H>
#include <x/w/impl/fonts/fontcharsetfwd.H>
#include <x/w/types.H>

LIBCXXW_NAMESPACE_START

//! A collection of fonts.

//! \see fontcollection

class fontcollectionObj : virtual public obj {

public:
	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	fontcollectionObj(const ref<implObj> &implArg) LIBCXX_HIDDEN;

	//! Destructor
	~fontcollectionObj() LIBCXX_HIDDEN;

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

	template<typename iter_type, typename callback_functor>
		void lookup(iter_type beg_iter,
			    const iter_type &end_iter,
			    callback_functor &&callback,
			    char32_t unprintable_char);

	template<typename iter_type>
		void load_glyphs(const iter_type &beg_iter,
				 const iter_type &end_iter,
				 char32_t unprintable_char);

	template<typename iter_type, typename lambda_type>
		void glyphs_width(iter_type &&beg_iter,
				  iter_type &&end_iter,
				  lambda_type &&lambda,
				  char32_t prev_char,
				  char32_t unprintable_char);

 private:

	//! type-erased lookup() implementation.

	void do_lookup(const function< bool(char32_t &) > &next,
		       const function< void(const freetypefont &)
		       > &callback,
		       char32_t unprintable_char);

};


LIBCXXW_NAMESPACE_END

#endif
