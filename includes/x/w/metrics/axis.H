/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_matrics_axis_H
#define x_w_matrics_axis_H

#include <x/w/types.H>
#include <x/exception.H>
#include <iostream>
#include <tuple>

LIBCXXW_NAMESPACE_START

//! The metrics namespace defines rules for computing a display element's size

//! A display elements horizontal and vertical size is specified by
//! \ref metrics::axis "INSERT_LIBX_NAMESPACE::w::metrics::axis", which
//! defines three values: minimum, preferred, and maximum. This specifies
//! the display element's minimum, preferred, and maximum size in the
//! given dimension.
//!
//! \ref metrics::rectangle "INSERT_LIBX_NAMESPACE::w::metrics::rectangle"
//! contains two members: horiz and vert. Both of them are \c axis
//! specifications, and this defines a display element's horizontal and
//! vertical dimensions.

namespace metrics {

	//! A single horizontal or a vertical axis.

	//! A display element specifies its requested minimum, desired, and
	//! maximum dimensions.

	class axis {

		//! Minimum size
		dim_t minimum_;

		//! Preferred size
		dim_t preferred_;

		//! Maximum size
		dim_t maximum_;

		static exception invalid_infinite(),
			invalid_minimum(),
			invalid_maximum();

		//! Sanity check the minimum, preferred, and maximum values.

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


		/////////////////////////////////////////////////////////
		//
		// We have "total", that we wish to divide into "into"
		// equal parts.

		template<typename functor_type>
		void divide(dim_t into,
			    functor_type &&functor)
		{
			auto preferred_increment = preferred() - minimum();
			auto maximum_increment = maximum() - preferred();

			dim_squared_t apportioned_minimum=0,
				apportioned_preferred=0,
				apportioned_maximum=0;

			for (dim_t i=0; i<into; ++i)
			{
				apportioned_minimum += minimum();
				apportioned_preferred += preferred_increment;
				apportioned_maximum += maximum_increment;

				auto new_minimum =
					apportioned_minimum / into;
				apportioned_minimum %= into;

				auto preferred_increment=
					apportioned_preferred / into;
				apportioned_preferred %= into;

				auto maximum_increment=
					apportioned_maximum / into;
				apportioned_maximum %= into;

				preferred_increment += new_minimum;
				maximum_increment   += preferred_increment;

				if (maximum() == dim_t::infinite())
					maximum_increment=dim_t::infinite();

				// We know that these values will all fit
				// into dim_t

				functor(axis{(dim_squared_t::value_type)
							new_minimum,
							(dim_squared_t::value_type)
							preferred_increment,
							(dim_squared_t::value_type)
							maximum_increment});
			}
		}

		//! Compute the total of all minimums of a sequence of axises.

		template<typename beg_iter_type,
			 typename end_iter_type>
		static dim_squared_t total_minimum(beg_iter_type beg_iter,
						   end_iter_type end_iter)
		{
			dim_squared_t n{0};

			while (beg_iter != end_iter)
			{
				axis &a=*beg_iter;
				n += a.minimum();
				++beg_iter;
			}

			return n;
		}

		//! Compute the total of all preferreds of a sequence of axises.

		template<typename beg_iter_type,
			 typename end_iter_type>
		static dim_squared_t total_preferred (beg_iter_type beg_iter,
						      end_iter_type end_iter)
		{
			dim_squared_t n{0};

			while (beg_iter != end_iter)
			{
				axis &a=*beg_iter;
				n += a.preferred();
				++beg_iter;
			}

			return n;
		}

		//! What total_maximum() returns.

		struct total_maximum_info {

			//! The total, excluding any infinite() maximums.

			//! This maybe dim_t::infinite() if the sum total
			//! of maximums exceeds the dim_t range().

			dim_squared_t sum_excluding_infinite=0;

			//! Whether the sequence has any infinite()s.
			bool has_infinites=false;
		};

		//! Compute the total of all maximums of a sequence of axises.

		template<typename beg_iter_type,
			 typename end_iter_type>
		static total_maximum_info total_maximum (beg_iter_type beg_iter,
							 end_iter_type end_iter)
		{
			dim_squared_t n{0};
			bool has_infinites=false;

			while (beg_iter != end_iter)
			{
				axis &a=*beg_iter;
				if (a.maximum() == dim_t::infinite())
				{
					has_infinites=true;
				}
				else
				{
					n += a.maximum();

					if ((dim_squared_t::value_type)n >
					    dim_t::infinite())
						n=dim_t::infinite();
				}
				++beg_iter;
			}

			return {n, has_infinites};
		}

		//! Remove infinite axises from a sequence.

		//! Each axis that has a maximum metric of infinite() has
		//! its maximum metric updated to the highest representable
		//! maximum value.

		template<typename beg_iter_type,
			 typename end_iter_type>
		static void remove_infinites (beg_iter_type beg_iter,
					      end_iter_type end_iter)
		{
			while (beg_iter != end_iter)
			{
				axis &a=*beg_iter;
				if (a.maximum() == dim_t::infinite())
				{
					a.maximum_=dim_t::infinite()-1;
				}
				++beg_iter;
			}
		}
	};

	//! Overload the \c << operator, to display the contents of an axis.
	std::ostream &operator<<(std::ostream &o, const axis &);
}

LIBCXXW_NAMESPACE_END

#endif
