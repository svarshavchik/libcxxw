/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "fonts/fontcharset_impl.H"
#include "fonts/fontconfig.H"

#include <x/exception.H>
LIBCXXW_NAMESPACE_START

namespace fontconfig {
#if 0
}
#endif

charsetObj::implObj::implObj(const config &configArg)
	: implObj(configArg, FcCharSetCreate(), true)
{
}

charsetObj::implObj::implObj(const config &configArg, FcCharSet *charsetArg,
			     bool autodestroyArg)
	: c(configArg), charset(charsetArg), autodestroy(autodestroyArg)
{
	if (!charsetArg)
		throw EXCEPTION("Could not create a character set map");
}

charsetObj::implObj::~implObj()
{
	charset_t::lock lock{charset};

	if (autodestroy)
		FcCharSetDestroy(*lock);
}

#if 0
{
#endif
}

LIBCXXW_NAMESPACE_END
