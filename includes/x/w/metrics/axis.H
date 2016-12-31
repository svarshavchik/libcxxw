/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_matrics_axis_H
#define x_w_matrics_axis_H

#include <x/w/types.H>
#include <x/exception.H>

LIBCXXW_NAMESPACE_START

//! The metrics namespace defines rules for indicating a display element's sizes.

namespace metrics {

	//! A single horizontal or a vertical axis.

	//! A display element specifies its requested minimum, desired, and
	//! maximum dimensions.

	class axis {

		dim_t minimum_;

		dim_t preferred_;

		dim_t maximum_;

		static exception invalid_infinite(),
			invalid_minimum(),
			invalid_maximum();

		static constexpr void validate(dim_t minimum, dim_t preferred,
					       dim_t maximum)
		{
			if (minimum == dim_t::infinite() ||
			    preferred == dim_t::infinite())
				throw invalid_infinite();

			if (minimum > preferred)
				throw invalid_minimum();

			if (preferred > maximum)
				throw invalid_maximum();
		}

	public:
		//! Constructor

		constexpr axis(dim_t minimum=dim_t(0),
			       dim_t preferred=dim_t(0),
			       dim_t maximum=dim_t(0))
			: minimum_{(validate(minimum, preferred, maximum),
				    minimum)},
			preferred_{preferred},
				maximum_{maximum}
			{
			}

		//! Update the metrics.

		constexpr void update(dim_t minimum, dim_t preferred,
				      dim_t maximum)
		{
			validate(minimum, preferred, maximum);
		}

		//! The minimum metric
		constexpr dim_t minimum() const { return minimum_; }

		//! The preferred metric
		constexpr dim_t preferred() const { return preferred_; }

		//! The maximum metric
		constexpr dim_t maximum() const { return maximum_; }

		//! Comparison operator
		constexpr bool operator==(const axis &o) const
		{
			return minimum_ == o.minimum_ &&
				maximum_ == o.maximum_ &&
				preferred_ == o.preferred_;
		}

		//! Comparison operator
		constexpr bool operator!=(const axis &o) const
		{
			return !operator==(o);
		}
	};
}

LIBCXXW_NAMESPACE_END

#endif
