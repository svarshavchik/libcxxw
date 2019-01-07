/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/impl/container.H"
#include "peephole/peephole_impl.H"
#include "peephole/peephole_layoutmanager_impl.H"
#include "x/w/button_event.H"

LIBCXXW_NAMESPACE_START

peepholeObj::implObj::implObj()=default;

peepholeObj::implObj::~implObj()=default;

bool peepholeObj::implObj::process_button_event(ONLY IN_THREAD,
						const button_event &be,
						xcb_timestamp_t timestamp)
{
	bool processed=false;

	get_container_impl().invoke_layoutmanager
		([&]
		 (const ref<peepholelayoutmanagerObj::implObj> &lm)
		 {
			 processed=lm->process_button_event(IN_THREAD, be,
							    timestamp);
		 });

	return processed;
}

LIBCXXW_NAMESPACE_END
