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

	class derivedaxis : public axis {

		dim_squared_t sum_of_preferreds{0};

		size_t counter{0};

		bool is_default=true;

	public:
		//! Constructor
		constexpr derivedaxis()=default;

		//! The next
		constexpr derivedaxis &operator()(const axis &a)
		{
			// Ignore default axi.
			if (a.minimum() == 0 && a.preferred() == 0 &&
			    a.maximum() == dim_t::infinite())
				return *this;

			++counter;
			sum_of_preferreds += a.preferred();

			if (is_default)
			{
				is_default=false;
				axis::operator=(a);
			}
			else
			{
				axis::operator&=(a);
				set_preferred( (dim_squared_t::value_type)
					       (sum_of_preferreds/counter));
			}
			return *this;
		}
	};
}

LIBCXXW_NAMESPACE_END

#endif
