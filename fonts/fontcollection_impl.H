/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef font_H
#define font_H

#include "fonts/fontconfigfwd.H"
#include "x/w/impl/fonts/fontcollection.H"
#include "x/w/impl/fonts/freetypefontobj.H"
#include "fonts/freetypefwd.H"
#include "fonts/fontsortedlist.H"
#include "x/w/impl/fonts/fontcharsetfwd.H"
#include "fonts/fontpatternfwd.H"
#include "x/w/screenfwd.H"
#include <x/mpobj.H>
#include <x/functional.H>

#include "fonts/cached_fontfwd.H"
#include "fonts/fontid_t.H"

#include <courier-unicode.h>

LIBCXXW_NAMESPACE_START

//! Internal implementation details of a \ref fontcollection "font collection".

class LIBCXX_PUBLIC fontcollectionObj::implObj : virtual public obj {

public:
	//! Constructor
	implObj(const font_id_t &font_idArg,
		const fontconfig::sortedlist &sortedlistArg,
		const const_screen &screenArg,
		const const_freetype &ftArg) LIBCXX_HIDDEN;

	//! Destructor
	~implObj() LIBCXX_HIDDEN;

	//! The collection's identifier
	const font_id_t font_id;

	//! The underlying list of fonts.
	const fontconfig::sortedlist sortedlist;

	//! My screen.
	const const_screen s;

	//! My freetype library.
	const const_freetype ft;

	//! A container for all the loaded fonts.
	class metadata_info {

	public:

		//! The loaded fonts.
		std::map<size_t, cached_font> opened_fonts;

	};

	//! Mutex protected container for the loaded fonts.
	typedef mpobj<metadata_info> metadata_t;

	//! Loaded fonts.
	metadata_t metadata;

	//! Compute the largest ascender.
	dim_t ascender();

	//! Compute the largest ascender.
	dim_t descender();

	//! Compute height by combining all known fonts ascender+descender values.

	dim_t height();

	//! Take the largest max advance from the default font.
	dim_t max_advance();

	//! Take nominal_width() from the default font.
	dim_t nominal_width();

	//! Check if the default font is fixed width.
	bool fixed_width();

	//! Implement lookup()

	void do_lookup(const function< bool(char32_t &) > &next,
		       const function< void(const freetypefont &)
		       > &callback,
		       char32_t unprintable_char);

	//! Return the best matching font's charset.
	fontconfig::const_charset default_charset();

 private:
	//! Make sure that the best matching font is open.

	void lookup_default(metadata_t::lock &lock);

	//! Convenience typedef
	typedef bool search_for_font_callback_t(const fontconfig::pattern &);

	//! Internal helper
	template<typename lambda_type>
		size_t search_for_font(metadata_t::lock &lock,
				       lambda_type &&lambda)
	{
		return do_search_for_font
			(lock,
			 make_function<search_for_font_callback_t>
			 (std::forward<lambda_type>(lambda)));
	}

	//! Type-erased search_for_font().
	size_t do_search_for_font(metadata_t::lock &lock,
				  const function<search_for_font_callback_t> &);
};

LIBCXXW_NAMESPACE_END

#endif
