/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "container_element.H"
#include "peephole/peephole_impl.H"
#include "peephole/peephole_layoutmanager_impl.H"
#include "x/w/button_event.H"

LIBCXXW_NAMESPACE_START

peepholeObj::implObj::~implObj()=default;

bool peepholeObj::implObj::process_button_event(IN_THREAD_ONLY,
						const button_event &be,
						xcb_timestamp_t timestamp)
{
	if (be.button == 4 || be.button == 5)
	{
		if (!activate_for(be))
			return true;

		invoke_layoutmanager
			([&]
			 (const ref<peepholeObj::layoutmanager_implObj> &lm)
			 {
				 if (be.button == 4)
					 lm->vert_scroll_low(IN_THREAD, be);
				 else
					 lm->vert_scroll_high(IN_THREAD, be);
			 });

		return true;
	}

	if (superclass_t::process_button_event(IN_THREAD, be, timestamp))
		return true;

	return false;
}

LIBCXXW_NAMESPACE_END
