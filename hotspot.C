/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "hotspot.H"
#include "child_element.H"
#include "xid_t.H"
#include "connection_thread.H"
#include "generic_window_handler.H"

LIBCXXW_NAMESPACE_START

hotspotObj::hotspotObj(const ref<implObj> &impl) : impl(impl)
{
}

hotspotObj::~hotspotObj()
{
	impl->get_hotspot_element().get_window_handler().thread()
		->run_as(RUN_AS,
			 [impl=this->impl]
			 (IN_THREAD_ONLY)
			 {
				 impl->hotspot_deinitialize(IN_THREAD);
			 });
}

void hotspotObj::on_activate(const hotspot_callback_t &callback)
{
	impl->on_activate(callback);
}

LIBCXXW_NAMESPACE_END
