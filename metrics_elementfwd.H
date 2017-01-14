/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef metrics_elementfwd_H
#define metrics_elementfwd_H

#include <x/ptrfwd.H>
#include "x/w/namespace.H"

LIBCXXW_NAMESPACE_START

namespace metrics {

	class elementObj;

	//! Metrics of a display element

	//! A reference-counted object with "horiz" and "vert" metrics::axis
	//! objects, specifying the display element's horizontal and
	//! vertical metrics.

	typedef ref<elementObj> element;

	//! Metrics of a display element

	//! \see element
	typedef ptr<elementObj> elementptr;

	//! Metrics of a display element

	//! \see element
	typedef const_ref<elementObj> const_element;

	//! Metrics of a display element

	//! \see element
	typedef const_ptr<elementObj> const_elementptr;
}

LIBCXXW_NAMESPACE_END

#endif
