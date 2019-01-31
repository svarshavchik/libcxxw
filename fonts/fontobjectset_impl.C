/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "fonts/fontobjectset_impl.H"
#include "fonts/fontconfig.H"

#include <x/exception.H>

LIBCXXW_NAMESPACE_START

namespace fontconfig {
#if 0
}
#endif

objectsetObj::implObj::implObj(const config &configArg)
	: c(configArg), set(FcObjectSetCreate())
{
	if (!*set_t::lock{set})
		throw EXCEPTION("Cannot create a font object set");

}

objectsetObj::implObj::~implObj()
{
	set_t::lock lock{set};

	FcObjectSetDestroy(*lock);
}

#if 0
{
#endif
}

LIBCXXW_NAMESPACE_END
