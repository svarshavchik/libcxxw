/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "fonts/fontcollection_impl.H"
#include "fonts/fontcharset.H"
#include "x/w/namespace.H"

LIBCXXW_NAMESPACE_START

fontcollectionObj::fontcollectionObj(const ref<implObj> &implArg): impl(implArg)
{
}

fontcollectionObj::~fontcollectionObj()
{
}

dim_t fontcollectionObj::height() const
{
	return impl->height();
}

dim_t fontcollectionObj::ascender() const
{
	return impl->ascender();
}

dim_t fontcollectionObj::descender() const
{
	return impl->descender();
}

dim_t fontcollectionObj::max_advance() const
{
	return impl->max_advance();
}

dim_t fontcollectionObj::nominal_width() const
{
	return impl->nominal_width();
}

bool fontcollectionObj::fixed_width() const
{
	return impl->fixed_width();
}

void fontcollectionObj::do_lookup(const function< bool(char32_t &) > &next,
				  const function< void(const freetypefont &) >
				  &callback,
				  char32_t unprintable_char)
{
	impl->do_lookup(next, callback, unprintable_char);
}

fontconfig::const_charset fontcollectionObj::default_charset() const
{
	return impl->default_charset();
}

LIBCXXW_NAMESPACE_END
