/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "hotspot.H"
#include "hotspot_bgcolor.H"
#include "x/w/impl/child_element.H"
#include "x/w/impl/background_color.H"

LIBCXXW_NAMESPACE_START

hotspot_bgcolorObj::implObj::implObj()=default;

hotspot_bgcolorObj::implObj::~implObj()=default;

void hotspot_bgcolorObj::implObj::temperature_changed(ONLY IN_THREAD,
						      const callback_trigger_t
						      &trigger)
{
	auto &hotspot_impl=get_hotspot_impl();
	auto &element=hotspot_impl.get_hotspot_element();

	switch (hotspot_impl.hotspot_temperature(IN_THREAD)) {
	case temperature::cold:
		{
			auto c=cold_color(IN_THREAD);

			if (!c)
			{
				element.remove_background_color(IN_THREAD);
				break;
			}
			element.set_background_color(IN_THREAD, *c);
		}
		break;
	case temperature::warm:
		element.set_background_color(IN_THREAD, warm_color(IN_THREAD));
		break;
	case temperature::hot:
		element.set_background_color(IN_THREAD, hot_color(IN_THREAD));
		break;
	}
}

LIBCXXW_NAMESPACE_END
