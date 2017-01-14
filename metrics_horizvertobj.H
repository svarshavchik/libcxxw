/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef metrics_horizvertobj_H
#define metrics_horizvertobj_H

#include "x/w/types.H"
#include "x/w/metrics/axis.H"
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

namespace metrics {

	//! A display element's metrics.

	//! Specifies a display element's horizontal and vertical sizes.

	class horizvertObj : virtual public obj {

	public:

		//! The horizontal dimensions.
		axis horiz;

		//! The vertical dimensions.
		axis vert;

		//! Constructor
		horizvertObj(dim_t h_minimum=dim_t(0),
			     dim_t h_preferred=dim_t(0),
			     dim_t h_maximum=dim_t(0),
			     dim_t v_minimum=dim_t(0),
			     dim_t v_preferred=dim_t(0),
			     dim_t v_maximum=dim_t(0));

		//! Destructor

		~horizvertObj();

		//! Comparison operator

		bool operator==(const horizvertObj &o) const
		{
			return horiz == o.horiz && vert == o.vert;
		}

		//! Comparison operator
		bool operator!=(const horizvertObj &o) const
		{
			return !operator==(o);
		}
	};
}

LIBCXXW_NAMESPACE_END

#endif
