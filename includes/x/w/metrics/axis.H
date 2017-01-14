/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_matrics_axis_H
#define x_w_matrics_axis_H

#include <x/w/types.H>
#include <x/exception.H>
#include <iostream>
#include <iterator>
#include <vector>
#include <numeric>
#include <algorithm>

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

		//! maximum() - minimum()

		constexpr dim_t spread() const { return maximum()-minimum(); }

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

		//! Update preferred value

		constexpr void set_preferred(dim_t p)
		{
			if (p < minimum_)
				p=minimum_;

			if (p > maximum_)
				p=maximum_;

			preferred_=p;
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

		//! Return a new axis, with an increased minimum()

		//! preferred() and maximum() adjusted as needed.

		axis increase_minimum_by(dim_t howmuch) const;

		//! Return a new axis, with a decreased maximum()

		//! maximum() does not get decreased below minimum().
		//! preferred() adjusted as needed.

		axis decrease_maximum_by(dim_t howmuch) const;

		//! Compute the maximum possible value this axis can increase_minimum_by

		constexpr dim_t max_increase_minimum_by() const
		{
			return dim_t(dim_t::infinite()-1) - minimum();
		}

		//! Compute the maximum possible value this axis can increase_minimum_by

		constexpr dim_t max_decrease_maximum_by() const
		{
			return maximum() == dim_t::infinite()
				? dim_t(0):maximum() - minimum();
		}

		//! Sort a vector of axises by minimum or maximum.

		//! The beginning and ending iterators must be random
		//! access iterators to axises.
		//!
		//! This returns a vector of iterators to each axis,
		//! sorted according to the comparison functor, which
		//! is expected to implement strict weak ordering on
		//! the individual axises, like by minimum or maximum.
		//!
		//! The relative order of axises that compare equally
		//! is preserved.

		template<typename iter_type, typename compare_functor>
		static std::vector<iter_type>
		sort_sequence_by(iter_type beg,
				 iter_type end,
				 compare_functor &&c)
		{
			std::vector<iter_type> v;

			v.resize(end-beg);
			std::iota(v.begin(), v.end(), beg);

			std::sort(v.begin(), v.end(),
				  [&]
				  (const auto &a_iter, const auto &b_iter)
				  {
					  const axis &a=*a_iter,
						  &b=*b_iter;

					  if (c(a, b)) return true;

					  if (c(b, a)) return false;

					  return (a_iter-beg) < (b_iter-beg);
				  });

			return v;
		}

		//! Invoke adjust_by() using max_increase_minimum_by()

		template<typename iter_type>
		static void adjust_minimums_by(iter_type beg_iter,
					       iter_type end_iter,
					       dim_squared_t byhowmuch)
		{
			adjust_by(beg_iter, end_iter, byhowmuch,
				  &axis::increase_minimum_by);
		}

		//! Invoke adjust_by() using max_increase_minimum_by()

		template<typename iter_type>
		static void adjust_maximums_by(iter_type beg_iter,
					       iter_type end_iter,
					       dim_squared_t byhowmuch)
		{
			adjust_by(beg_iter, end_iter, byhowmuch,
				  &axis::decrease_maximum_by);
		}

		//! Adjust a sequence of axises by a total amount, fairly.

		//! Applies a total adjustment to a sequence of axises
		//! in a fair manner.
		//!
		//! If the adjustment is to the maximum()s, assumes that none
		//! of the axises have infinite() maximums, because whatever
		//! the reason the adjustment is made, it should've taken
		//! advantage of infinite() maximums, first.
		//!
		//! The adjustment is applied first to axises with the
		//! greatest difference between their minimum and maximum
		//! values. As much as possible, the same adjustment gets
		//! applied to all axises with the same difference between
		//! their minimum and maximum values.

		template<typename iter_type>
		static void adjust_by(//! RandomAccessIterator, start of seq
				      iter_type beg_iter,
				      //! RandomAccessIterator, end of seq
				      iter_type end_iter,
				      //! Total adjustment to apply.
				      dim_squared_t byhowmuch,
				      //! Either increase_minimum_by or decrease_maximum_by
				      axis (axis::*adjust)(dim_t) const)
		{
			if (beg_iter == end_iter)
				return;

			// Sort by largest spread first.
			auto sorted_sequence=
				sort_sequence_by
				(beg_iter,
				 end_iter,
				 []
				 (const auto &a, const auto &b)
				 {
					 return a.spread() > b.spread();
				 });

			// Repeatedly try to adjust the spread, until
			// byhowmuch gets reduced to 0.
			//
			// sorted_sequence is now ordered by largest spread
			// first.

			while (byhowmuch > 0)
			{
				const axis &a=*sorted_sequence.front();

				dim_t largest_spread=a.spread();

				// Otherwise, find the 2nd largest spread.

				auto next_largest_spread_iter=
					std::find_if
					(sorted_sequence.begin(),
					 sorted_sequence.end(),
					 [largest_spread]
					 (const auto &iter)
					 {
						 const axis &a=*iter;

						 return a.spread()
						 != largest_spread;
					 });

				// If all remaining axises have the same
				// spread, divide the pain equally. Divide
				// byhowmuch by the number of axises.

				dim_squared_t adjust_by;

				if (next_largest_spread_iter
				    == sorted_sequence.end())
				{
					adjust_by=((dim_squared_t::value_type)
						   byhowmuch
						   + sorted_sequence.size()-1)
						 / sorted_sequence.size();
				}
				else
				{
					// Otherwise use the difference between
					// the largest spread and the next
					// largest spread, adjusting each
					// axis by the difference, essentially
					// reducing the axises with the largest
					// spread to the next-largest spread.

					const axis &a=**next_largest_spread_iter;
					dim_t next_largest_spread=a.spread();

					adjust_by=(dim_t::value_type)
						(largest_spread-
						 next_largest_spread);
				}

				// Iterate over all axises with the largest
				// spread, and adjust_by them.

				for (auto p=sorted_sequence.begin();
				     p != next_largest_spread_iter; ++p)
				{
					// Adjust by the lesser of adjust_by
					// and the remaining adjustment to
					// be applied, in sum.

					dim_squared_t doit=adjust_by;

					if (doit > byhowmuch)
						doit=byhowmuch;

					if (doit >= dim_t::infinite())
						doit=dim_t::infinite()-1;


					// Use adjust() to apply the adjustment.
					axis &current_metrics= **p;

					current_metrics=
						(current_metrics.*adjust)
						((dim_squared_t::value_type)
						 doit);

					byhowmuch -= doit;

					// Have we applied the adjustment in
					// total?
					if (byhowmuch == 0)
						break;
				}
			}
		}

		//! Adjust this axis on account of another axis.

		//! This uses adjust_minimums_by() and adjust_maximums_by()
		//! only on this axis in order for it to accomodate another
		//! axis's requirements, as much as possible.

		axis &operator &= (const axis &other);

	};

	//! Overload the \c << operator, to display the contents of an axis.
	std::ostream &operator<<(std::ostream &o, const axis &);
}

LIBCXXW_NAMESPACE_END

#endif
