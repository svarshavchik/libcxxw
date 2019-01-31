/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef metrics_axis_H
#define metrics_axis_H

#include "x/w/metrics/derivedaxis.H"
#include <x/derivedvalue.H>

LIBCXXW_NAMESPACE_START

namespace metrics {

	//! Wrap a derivedaxis computation into a derivedvalue container.

	typedef derivedvalue<axis, axis> derived_axis_obj;

	//! Create a derived axis value calculation object.

	derived_axis_obj create_derived_axis_obj();
}

LIBCXXW_NAMESPACE_END

#endif
