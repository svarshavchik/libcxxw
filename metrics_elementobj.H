/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef metrics_elementobj_H
#define metrics_elementobj_H

#include <x/w/metrics/rectangle.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

namespace metrics {

	//! Element metrics are nothing but a rectangle object.

	class elementObj : virtual public obj, public rectangle {

	public:
		using rectangle::rectangle;

		~elementObj();
	};
}

LIBCXXW_NAMESPACE_END

#endif
