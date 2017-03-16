/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef cached_font_H
#define cached_font_H

#include "fonts/freetypefont.H"
#include "fonts/fontcharsetfwd.H"
#include "fonts/cached_fontfwd.H"

#include <x/ref.H>

// Individually cached fonts

LIBCXXW_NAMESPACE_START

//! A cached font

//! When a \ref freetypefont "freetypefont" is loaded we cache it in the
//! \ref connection "connection", for reuse.
//!
//! We also cache the font's character set coverage.

class LIBCXX_HIDDEN cached_fontObj : virtual public obj {

 public:

	//! The actual font.
	const freetypefont f;

	//! Font's character set coverage
	const fontconfig::const_charset charset;

	//! Constructor
	cached_fontObj(const freetypefont &fArg,
		      const fontconfig::const_charset &charsetArg);

	//! Destructor
	~cached_fontObj();
};

LIBCXXW_NAMESPACE_END

#endif
