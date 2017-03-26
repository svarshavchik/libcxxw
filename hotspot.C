/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "hotspot.H"

LIBCXXW_NAMESPACE_START

hotspotObj::hotspotObj(const ref<implObj> &impl) : impl(impl)
{
}

hotspotObj::~hotspotObj()=default;

void hotspotObj::on_activate(const hotspot_callback_t &callback)
{
	impl->on_activate(callback);
}

LIBCXXW_NAMESPACE_END
