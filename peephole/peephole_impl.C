/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/impl/container_element.H"
#include "peephole/peephole_impl.H"
#include "peephole/peephole_layoutmanager_impl.H"
#include "x/w/button_event.H"

LIBCXXW_NAMESPACE_START

peepholeObj::implObj::~implObj()=default;

bool peepholeObj::implObj::process_button_event(ONLY IN_THREAD,
						const button_event &be,
						xcb_timestamp_t timestamp)
{
	bool processed=false;

	invoke_layoutmanager
		([&]
		 (const ref<peepholeObj::layoutmanager_implObj> &lm)
		 {
			 processed=lm->process_button_event(IN_THREAD, be,
							    timestamp);
		 });

	if (processed)
		return true;

	return superclass_t::process_button_event(IN_THREAD, be, timestamp);
}

LIBCXXW_NAMESPACE_END
