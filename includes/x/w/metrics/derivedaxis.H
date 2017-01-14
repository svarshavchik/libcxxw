/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_matrics_derivedaxis_H
#define x_w_matrics_derivedaxis_H

#include <x/w/metrics/axis.H>

LIBCXXW_NAMESPACE_START

namespace metrics {

	//! An axis metric that's derived from other axis metrics.

	//! This is used to take one or more existing
	//! \ref axis "axis metrics", and compute a single axis metric
	//! that attempts to accomodate them as best as it can be, under
	//! the circumstances.
	//!
	//! \code
	//!   std::vector<INSERT_LIBX_NAMESPACE::w::metrics::axis> axi;
	//!
	//!   INSERT_LIBX_NAMESPACE::w::metrics::axis result=std::for_each(axi.begin(), axi.end(), INSERT_LIBX_NAMESPACE::w::metrics::derivedaxis());
	//! \endcode
	//!
	//! The resulting derived axis is computed as possible:
	//!
	//! - The minimum value is the largest minimum value.
	//!
	//! - The maximum value is the smallest maximum value.
	//!
	//! - If the calculated minimum value is greater than the calculated
	//! maximum value, the maximum is set to the minimum.
	//!
	//! - The preferred value is the average of all preferred values,
	//! adjusted to be no less than the computed minimum value, and
	//! no larger than the computed maximum value.

	class derivedaxis {

		dim_t largest_minimum{0};

		dim_squared_t sum_of_preferreds{0};

		dim_t smallest_maximum{dim_t::infinite()};

		size_t counter{0};

	public:
		//! Constructor
		constexpr derivedaxis()=default;

		//! The next
		constexpr derivedaxis &operator()(const axis &a)
		{
			if (a.minimum() > largest_minimum)
				largest_minimum=a.minimum();

			if (a.maximum() < smallest_maximum)
				smallest_maximum=a.maximum();

			sum_of_preferreds += a.preferred().n;
			++counter;
		}

		constexpr operator axis() const
		{
			auto minimum=largest_minimum;

			auto maximum=smallest_maximum;

			if (minimum > maximum)
				maximum=minimum;

			auto preferred=dim_t{(dim_t::value_type)
					     (counter ?
					      (sum_of_preferreds /
					       counter).n:0)};

			if (preferred < minimum)
				preferred=minimum;

			if (preferred > maximum)
				preferred=maximum;

			if (counter == 0)
				maximum=dim_t{0};

			return {minimum, preferred, maximum};
		}
	};
}

LIBCXXW_NAMESPACE_END

#endif
