/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "fonts/fontcollection_impl.H"
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

void fontcollectionObj::do_lookup(const function< bool(char32_t &) > &next,
				  const function< void(const freetypefont &) >
				  &callback)
{
	impl->do_lookup(next, callback);
}

LIBCXXW_NAMESPACE_END