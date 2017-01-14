/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_matrics_rectangle_H
#define x_w_matrics_rectangle_H

#include <x/w/types.H>
#include <x/w/metrics/axis.H>

LIBCXXW_NAMESPACE_START

namespace metrics {

	//! A display element's metrics.

	//! Specifies a display element's horizontal and vertical sizes.

	class rectangle {

	public:

		//! The horizontal dimensions.
		axis horiz;

		//! The vertical dimensions.
		axis vert;

		//! Constructor
		constexpr rectangle(dim_t h_minimum=dim_t(0),
				    dim_t h_preferred=dim_t(0),
				    dim_t h_maximum=dim_t(0),
				    dim_t v_minimum=dim_t(0),
				    dim_t v_preferred=dim_t(0),
				    dim_t v_maximum=dim_t(0))
		: horiz(h_minimum, h_preferred, h_maximum),
			vert(v_minimum, v_preferred, v_maximum)
		{
		}


		//! Comparison operator

		constexpr bool operator==(const rectangle &o) const
		{
			return horiz == o.horiz && vert == o.vert;
		}

		//! Comparison operator
		constexpr bool operator!=(const rectangle &o) const
		{
			return !operator==(o);
		}
	};
}

LIBCXXW_NAMESPACE_END

#endif
