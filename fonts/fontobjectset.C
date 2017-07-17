/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "fonts/fontobjectset_impl.H"
#include "fonts/fontconfig.H"
#include <x/exception.H>
#include <algorithm>

LIBCXXW_NAMESPACE_START

namespace fontconfig {
#if 0
}
#endif

objectsetObj::objectsetObj(const ref<implObj> &implArg) : impl(implArg)
{
}

objectsetObj::~objectsetObj()
{
}

void objectsetObj::add(const std::string_view &s)
{
	char buffer[s.size()+1];

	std::copy(s.begin(), s.end(), buffer);
	buffer[s.size()]=0;

	implObj::set_t::lock lock{impl->set};

	if (!FcObjectSetAdd(*lock, buffer))
		throw EXCEPTION("FcObjectSetAdd failed");
}


#if 0
{
#endif
}

LIBCXXW_NAMESPACE_END
