/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "fonts/cached_font.H"
#include "x/w/impl/fonts/freetypefont.H"
#include "fonts/fontcharset.H"

#include "x/w/namespace.H"

LIBCXXW_NAMESPACE_START

cached_fontObj::cached_fontObj(const freetypefont &fArg,
			     const fontconfig::const_charset &charsetArg)
	: f(fArg),
	  charset(charsetArg)
{
}

cached_fontObj::~cached_fontObj() noexcept
{
}

LIBCXXW_NAMESPACE_END
