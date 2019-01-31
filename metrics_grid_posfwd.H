/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef metrics_grid_grid_posfwd_H
#define metrics_grid_grid_posfwd_H

#include "x/w/namespace.H"
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

namespace metrics {

	class LIBCXX_HIDDEN grid_posObj;

	//! Everything you want to know about an element in a grid.

	//! Such as:
	//!
	//! - horiz_pos, vert_pos: grid_axispos - the element's horizontal
	//! and vertical position.
	//!
	//! - metrics: a metrics::element object, which contains
	//!  horiz and vert metrics::axis objects.

	typedef ref<grid_posObj> grid_pos;

}

LIBCXXW_NAMESPACE_END

#endif
