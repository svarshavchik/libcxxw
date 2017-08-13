/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "menu/menulayoutmanager_impl.H"

LIBCXXW_NAMESPACE_START

menulayoutmanagerObj::menulayoutmanagerObj(const ref<implObj> &impl)
	: listlayoutmanagerObj(impl),
	  impl(impl)
{
}

menulayoutmanagerObj::~menulayoutmanagerObj()=default;

LIBCXXW_NAMESPACE_END
