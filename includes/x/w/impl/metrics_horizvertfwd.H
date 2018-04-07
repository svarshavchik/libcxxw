/*
** Copyright 2017-2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_metrics_horizvertfwd_H
#define x_w_impl_metrics_horizvertfwd_H

#include <x/ptrfwd.H>
#include <x/w/namespace.H>

LIBCXXW_NAMESPACE_START

namespace metrics {

	class horizvertObj;

	//! Metrics of a display element

	//! A reference-counted object with "horiz" and "vert" metrics::axis
	//! objects, specifying the display element's horizontal and
	//! vertical metrics.

	typedef ref<horizvertObj> horizvert;

	//! Metrics of a display element

	//! \see horizvert
	typedef ptr<horizvertObj> horizvertptr;

	//! Metrics of a display element

	//! \see horizvert
	typedef const_ref<horizvertObj> const_horizvert;

	//! Metrics of a display element

	//! \see horizvert
	typedef const_ptr<horizvertObj> const_horizvertptr;
}

LIBCXXW_NAMESPACE_END

#endif
