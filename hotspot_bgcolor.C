/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "hotspot_bgcolor.H"
#include "hotspot.H"

LIBCXXW_NAMESPACE_START

hotspot_bgcolorObj::hotspot_bgcolorObj(const ref<implObj> &impl)
	: hotspotObj(ref<hotspotObj::implObj>(&impl->get_hotspot_impl())),
	  impl(impl)
{
}

hotspot_bgcolorObj::~hotspot_bgcolorObj()=default;

LIBCXXW_NAMESPACE_END
