/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "hotspot.H"
#include "x/w/impl/child_element.H"
#include "xid_t.H"
#include "connection_thread.H"
#include "run_as.H"

LIBCXXW_NAMESPACE_START

hotspotObj::hotspotObj(const ref<implObj> &impl) : impl(impl)
{
}

hotspotObj::~hotspotObj()=default;

void hotspotObj::on_activate(const hotspot_callback_t &callback)
{
	impl->on_activate(callback);
}

void hotspotObj::on_activate(ONLY IN_THREAD,
			     const hotspot_callback_t &callback)
{
	impl->on_activate(IN_THREAD, callback);
}

LIBCXXW_NAMESPACE_END
