/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "x/w/text_hotspot.H"

LIBCXXW_NAMESPACE_START

text_hotspotObj::text_hotspotObj(const functionref<text_param
				 (THREAD_CALLBACK,
				  const text_event_t &)> &event)
	:event{event}
{
}

text_hotspotObj::~text_hotspotObj() noexcept
{
}

LIBCXXW_NAMESPACE_END
