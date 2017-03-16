/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "fonts/fontconfig_impl.H"

LIBCXXW_NAMESPACE_START

namespace fontconfig {
#if 0
}
#endif


configObj::implObj::implObj()
	:fc(FcInitLoadConfigAndFonts())
{
	fontconfig_t::lock lock{fc};

	FcConfigSetRescanInterval(*lock, 0);
}

configObj::implObj::~implObj()
{
	fontconfig_t::lock lock{fc};

	FcConfigDestroy(*lock);
}

#if 0
{
#endif
}

LIBCXXW_NAMESPACE_END

