/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_metrics_mmaxis_H
#define x_w_metrics_mmaxis_H

#include <x/w/metrics/axis.H>
#include <x/w/metrics/mmaxisfwd.H>
#include <x/w/screenfwd.H>

LIBCXXW_NAMESPACE_START

namespace metrics {

	//! A single horizontal or a vertical axis in millimeters.

	//! Implements a subset of \ref axis "axis", expressing the minimum,
	//! preferred, and positive values in millimeters.
	//!
	//! This is merely a place to store the axis expressed in millimeters,
	//! until they can be converted to internal pixel counts.

	class mmaxis {

		//! Minimum size
		double minimum_;

		//! Preferred size
		double preferred_;

		//! Maximum size
		double maximum_;

	public:
		static constexpr double infinite() { return NAN; }

		//! Sanity check the minimum, preferred, and maximum values.

		static constexpr void validate(double minimum, double preferred,
					       double maximum)
		{
			if (std::isnan(minimum) || std::isnan(preferred))
				throw axis::invalid_infinite();

			if (minimum < 0 || minimum > preferred)
				throw axis::invalid_minimum();

			if (!std::isnan(maximum) && preferred > maximum)
				throw axis::invalid_maximum();
		}

	public:
		//! Constructor
		constexpr mmaxis(double size) : mmaxis(size, size, size)
		{
		}

		//! Constructor, the dimensions are specified in millimeters.

		constexpr mmaxis(double minimum,
				 double preferred,
				 double maximum=infinite())
		: minimum_{(validate(minimum, preferred, maximum), minimum)},
			preferred_{preferred},
			maximum_{maximum}
		{
		}

		//! The minimum metric
		constexpr double minimum() const { return minimum_; }

		//! The preferred metric
		constexpr double preferred() const { return preferred_; }

		//! The maximum metric
		constexpr double maximum() const { return maximum_; }
	};
}

LIBCXXW_NAMESPACE_END

#endif
