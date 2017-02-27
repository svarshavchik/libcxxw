/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "metrics_grid.H"
#include "metrics_grid_pos.H"
#include "metrics_grid_axisrange.H"

#include <functional>
#include <iterator>
#include <set>
#include <limits>

//#define DEBUG

#ifdef DEBUG
#include <iostream>
#endif

LIBCXXW_NAMESPACE_START
namespace metrics {
#if 0
}
#endif


const struct grid_major h_grid LIBCXX_HIDDEN ={
	&grid_posObj::horiz_pos,
	&horizvertObj::horiz,
	&pos_axis_padding::total_horiz_padding,
	&v_grid,
};

const struct grid_major v_grid LIBCXX_INTERNAL ={
	&grid_posObj::vert_pos,
	&horizvertObj::vert,
	&pos_axis_padding::total_vert_padding,
	&h_grid,
};

// For speed, we also want a vector-like view into grid_metrics_t

typedef std::vector<std::reference_wrapper<axis>> grid_metrics_refvec_t;

static grid_metrics_refvec_t
create_grid_metrics_refvec(grid_metrics_t &m,
			   grid_xy start_pos,
			   grid_xy end_pos,
			   const std::set<grid_xy> &all_grid_positions,
			   std::set<grid_xy> &missing_pos) LIBCXX_INTERNAL;

// We have an element with the given start/ending position on a grid's axis.
// grid_metrics_t are the metrics for the computed positions on the axis.
//
// Not every position start-end may exist. If a display element spans
// across the given axis position, that axis position may not yet actually
// been computed yet. all_grid_positions enumerates all grid positions
// that exist.
//
// Take start-end all_grid_positions that are already in the grid_metrics_t,
// and return a grid_metrics_refvec_t, for calculations.
//
// And all_grid_positions that do not already exist, put them into
// missing_pos.

static grid_metrics_refvec_t
create_grid_metrics_refvec(grid_metrics_t &m,
			   grid_xy start_pos,
			   grid_xy end_pos,
			   const std::set<grid_xy> &all_grid_positions,
			   std::set<grid_xy> &missing_pos)
{
	missing_pos.clear();

	grid_metrics_refvec_t v;

	grid_metrics_t::iterator b=m.lower_bound(start_pos);
	grid_metrics_t::iterator e=m.upper_bound(end_pos);

	v.reserve(std::distance(b, e));

	if (b == e) // Edge case. Nothing start-end exists already.
	{
		// End result: copy start-end from all_grid_positions into
		// missing_pos. Everything is missing.

		missing_pos.insert(all_grid_positions.lower_bound(start_pos),
				   all_grid_positions.upper_bound(end_pos));
		return v;
	}

	// start_pos is the next grid position that we expect to iterate
	// over.
	while (1)
	{
		if (b->first != start_pos)
		{
			// End, it's not there. Everything between start_pos,
			// and up to prev_pos, is missing.
			auto prev_pos=b->first-1;

			missing_pos.insert(all_grid_positions.lower_bound(start_pos),
					   all_grid_positions.upper_bound(prev_pos));
		}

		start_pos=b->first;

		v.push_back(b->second);

		if (++b == e)
		{
			// This was the last position. One more edge condition
			// to check: missing positions at the end of the
			// start-end range.

			if (start_pos != end_pos)
			{
				++start_pos;
				missing_pos.insert(all_grid_positions
						   .lower_bound(start_pos),
						   all_grid_positions
						   .upper_bound(end_pos));
			}
			break;
		}
		++start_pos;
	}

	return v;
}


// Ok, we want to factor in a grid element that spans the given range,
// and has the given metrics.

void apply_metrics(grid_metrics_t &m,
		   const std::set<grid_xy> &all_grid_positions,
		   const grid_axisrange &r,
		   const axis &aa,
		   dim_t axis_padding)
{
	// Ok, here's how big this grid element is.

	dim_t a_minimum=aa.minimum();
	dim_t a_preferred=aa.preferred();
	dim_t a_maximum=aa.maximum();

	// Adjust for the padding.

	if (a_maximum < dim_t::infinite())
	{
		a_maximum=dim_t::truncate(a_maximum+axis_padding);

		if (a_maximum == dim_t::infinite())
			--a_maximum;
	}

	a_preferred=dim_t::truncate(a_preferred+axis_padding);

	if (a_preferred == dim_t::infinite())
		--a_preferred;

	if (a_preferred > a_maximum)
		a_preferred=a_maximum;

	a_minimum=dim_t::truncate(a_minimum+axis_padding);

	if (a_minimum > a_preferred)
		a_minimum=a_preferred;

	std::set<grid_xy> missing_pos;

#ifdef DEBUG
	std::cout << "Processing " << a << ": "
		  << r.start << "-" << r.end << std::endl;
#endif
	// Take every position we're dealing with, here.
	auto v=create_grid_metrics_refvec(m,
					  r.start,
					  r.end,
					  all_grid_positions,
					  missing_pos);

#ifdef DEBUG
	for (const axis &c:v)
	{
		std::cout << "   column: " << c << std::endl;
	}

#endif

	auto total_minimum=axis::total_minimum(v.begin(), v.end());
	auto total_preferred=axis::total_preferred(v.begin(), v.end());
	auto total_maximum=axis::total_maximum(v.begin(), v.end());

	// This elements spans some positions that have not been calculated yet.
	//
	// If this element's minimum is greater than total_minimum, the
	// additional minimum can be handled by all the new columns.

	if (!missing_pos.empty())
	{
		// Otherwise the new columns' minimums are 0. This element
		// already spans columns that meet the required minimum.
		dim_t new_minimum=
			total_minimum < (dim_t::value_type)a_minimum
					? a_minimum-total_minimum
					: dim_t(0);

		// For now, assume that the new columns' preferred is the
		// same as the minimums.
		dim_t new_preferred=new_minimum;

#ifdef DEBUG
		std::cout << "new_minimum " << new_minimum << std::endl;
#endif

		// If this column's preferred is larger than the
		// existing columns' preferred total, the additional
		// preferred can be handled by the new columns.

		if (total_preferred < (dim_t::value_type)a_preferred)
		{
			new_preferred=a_preferred-total_preferred;
			if (new_preferred < new_minimum)
				new_preferred=new_minimum;
		}

		// Opening bid: the maximum size is infinite.
		dim_t new_maximum=dim_t::infinite();

		// But if it's not:
		if (a_maximum != dim_t::infinite())
		{
			// If the existing calculated columns that are
			// spanned by this range have infinite maximums,
			// sadly they can no longer be, so remove the
			// infinites, fornow.

			if (total_maximum.has_infinites)
			{
				axis::remove_infinites(v.begin(), v.end());
				total_maximum=axis::total_maximum(v.begin(),
								  v.end());
			}

			// Opening bid: see if the total_maximum for this
			// element can be removed from the existing positions
			// spanned by this element.
			axis::adjust_maximums_by(v.begin(), v.end(),
						 total_maximum.sum_excluding_infinite);
			// Recompute the revised total_maximum.
			total_maximum=axis::total_maximum(v.begin(),
							  v.end());

			// Pay attention: if the new positions' minimum plus
			// total maximum of existing positions is less than
			// the new element's total maximum, this means that
			// the new positions' maximum can be more than the
			// minimum, and still make everyone happy.
			new_maximum=new_minimum;

			if (new_maximum + total_maximum.sum_excluding_infinite
			    < (dim_t::value_type)a_maximum)
			{
				new_maximum=a_maximum - total_maximum.sum_excluding_infinite;
			}
		}

		// Make sure that preferred value is kosher, with respect
		// to the newly-computed maximum.

		if (new_preferred > new_maximum)
			new_preferred=new_maximum;

#ifdef DEBUG
		std::cout << "new_preferred " << new_preferred << std::endl;
		std::cout << "new_maximum " << new_maximum << std::endl;
#endif

		// And here's the sum total of then new position.
		axis new_axis{new_minimum, new_preferred, new_maximum};

		// Use divide() to divide new_axis into roughly equal parts,
		// with each part becoming the next missing_pos.
		auto p=missing_pos.begin();

		new_axis.divide(missing_pos.size(),
				[&]
				(const auto &a)
				{
#ifdef DEBUG
					std::cout << "Inserting "
						  << a << " into "
						  << *p << std::endl;
#endif
					m.insert({*p++, a});
				});

		// We now need to recalculate the statistics, given the
		// new state of the world.
		v=create_grid_metrics_refvec(m,
					     r.start,
					     r.end,
					     all_grid_positions,
					     missing_pos);

		total_minimum=axis::total_minimum(v.begin(), v.end());
		total_maximum=axis::total_maximum(v.begin(), v.end());
#ifdef DEBUG
		std::cout << "Updated total_minimum: " << total_minimum
			  << " from " << r.start << "-" << r.end << std::endl;

		for (const axis &c:v)
		{
			std::cout << "   column: " << c << std::endl;
		}
#endif
	}

	// Ok, at this point, all grid positions spanned by the element
	// exist. See if the existing display positions' total_minimum is
	// less than the new element's minimum. If so, they need to be
	// adjusted.
	if (total_minimum < (dim_t::value_type)a_minimum)
	{
		axis::adjust_minimums_by(v.begin(), v.end(),
					 (dim_t::value_type)a_minimum
					 -total_minimum);

		// adjust_minimums_by() could've updated some position's
		// maximum value too, so recalculate the total_maximum.
		total_maximum=axis::total_maximum(v.begin(), v.end());
	}

	// If the new element's maximum size is not infinite, we need to
	// enforce it.
	if (a_maximum != dim_t::infinite())
	{
		// The first step towards enforcing it is to get rid of
		// any existing infinite positions.
		if (total_maximum.has_infinites)
		{
			axis::remove_infinites(v.begin(), v.end());
			total_maximum=axis::total_maximum(v.begin(), v.end());
		}

		// Use adjust_maximums_by(), to try to reduce the maximum
		// sizes.
		if (total_maximum.sum_excluding_infinite
		    > (dim_t::value_type)a_maximum)
		{
			axis::adjust_maximums_by(v.begin(), v.end(),
						 total_maximum
						 .sum_excluding_infinite -
						 (dim_t::value_type)a_maximum
						 );
		}
	}
}

static void prorated_size(const grid_metrics_t &m,
			  const std::vector<grid_xy> &sorted_xy,

			  grid_sizes_t &s,

			  dim_squared_t share,
			  dim_squared_t total,

			  bool (*do_apply)(const axis &),
			  dim_t (*towards)(const axis &)) LIBCXX_INTERNAL;

static bool apply_all(const axis &) LIBCXX_INTERNAL;
static bool apply_noninfinites(const axis &a) LIBCXX_INTERNAL;
static dim_t apply_minimum_size(const axis &a) LIBCXX_INTERNAL;
static dim_t apply_preferred_size(const axis &a) LIBCXX_INTERNAL;
static dim_t apply_maximum_size(const axis &a) LIBCXX_INTERNAL;

// prorated_size: apply to all axises.

static bool apply_all(const axis &)
{
	return true;
}

// prorated_size: apply to all non-infinite maximum axises.

static bool apply_noninfinites(const axis &a)
{
	return a.maximum() != dim_t::infinite();
}

// prorated_size: apply to all infinite maximum axises.

static bool apply_infinites(const axis &a)
{
	return a.maximum() == dim_t::infinite();
}

// prorated_size: apply in proportion to each axis's minimum size

static dim_t apply_minimum_size(const axis &a)
{
	return a.minimum();
}

// prorated_size: apply in proportion to each axis's additional preferred size

static dim_t apply_preferred_size(const axis &a)
{
	return a.preferred()-a.minimum();
}

// prorated_size: apply in proportion to each axis's additional maximum size

static dim_t apply_maximum_size(const axis &a)
{
	return a.maximum()-a.preferred();
}

// Fixed proration factor of 1 for infinite maximum axises.

static dim_t apply_infinite_size(const axis &a)
{
	return 1;
}

bool calculate_grid_size(const grid_metrics_t &m,
			 grid_sizes_t &current_sizes,
			 dim_t target_size)
{
	// Clear, and value-initialize a new current_sizes to 0.

	// At the same time, sort the positions in order.

	grid_sizes_t s;

	std::vector<grid_xy> sorted_xy;

	sorted_xy.reserve(m.size());
	for (const auto &key:m)
	{
		s[key.first]; // Value initialization
		sorted_xy.push_back(key.first);
	}

	std::sort(sorted_xy.begin(), sorted_xy.end());

	// Sum total of everyone's minimums
	dim_squared_t apply_total_minimum=0;

	// The sum total of the difference between preferred and minimum.

	dim_squared_t apply_total_preferred=0;

	// The sum total of the difference between maximum and preferred,
	// but only for the columns that do not have an infinite maximum.

	dim_squared_t apply_total_maximum=0;

	// How many columns with infinite maximum size.
	size_t n_infinites=0;

	std::for_each(m.begin(), m.end(),
		      [&]
		      (const auto &keyvalue)
		      {
			      apply_total_minimum +=
				      apply_minimum_size(keyvalue.second);
			      apply_total_preferred +=
				      apply_preferred_size(keyvalue.second);

			      if (apply_noninfinites(keyvalue.second))
				      apply_total_maximum +=
					      apply_maximum_size(keyvalue
								 .second);
			      else
				      ++n_infinites;
		      });

	// We expect that target_size will always be at least as much
	// as apply_total_minimum.

	prorated_size(m, sorted_xy, s, apply_total_minimum, apply_total_minimum,
		      &apply_all, &apply_minimum_size);

	// After the minimum, apply the preferred amounts.

	dim_squared_t remaining=
		target_size+(dim_t)0 > apply_total_minimum
		? target_size+dim_t(0)-apply_total_minimum
		: dim_squared_t(0);

	dim_squared_t apply=remaining;

	if (apply > apply_total_preferred)
		apply=apply_total_preferred;
	prorated_size(m, sorted_xy, s, apply, apply_total_preferred, &apply_all,
		      &apply_preferred_size);

	remaining -= apply;
	apply=remaining;

	// Now, the maximum (excluding infinites)

	if (apply > apply_total_maximum)
		apply=apply_total_maximum;

	prorated_size(m, sorted_xy, s, apply, apply_total_maximum,
		      &apply_noninfinites,
		      &apply_maximum_size);

	remaining -= apply;
	apply=remaining;

	// If there are any maximum-infinite columns. "apply" is what's
	// left after satisfying all others' maximum sizes.
	//
	// So what we do is pass "apply" to prorated_size() as the share
	// to apply, as usual. But specify n_infinites for, supposedly, the
	// total amount being applied. apply_infinite_size() returns 1, so
	// each maximum axis receives 1/n_infinites of "apply".

	if (n_infinites)
		prorated_size(m, sorted_xy, s, apply, n_infinites,
			      apply_infinites, apply_infinite_size);

	// Now that all dimensions have been computed, calculate the
	// starting coordinates

	coord_t p=0;

	for (const auto &xy: sorted_xy)
	{
		auto &dimpos=s.at(xy);

		std::get<coord_t>(dimpos)=p;

		auto next_p=(coord_squared_t::value_type)
			(p+std::get<dim_t>(dimpos));

		if (p.overflows(next_p))
			next_p=std::numeric_limits<coord_t::value_type>::max();
		p=next_p;
	}

	if (s == current_sizes)
		return false;

	current_sizes=s;

	return true;
}

// Ok, we are about to distribute 'share' of actual size to grid_sizes_t,
// which is equal to or less than 'total', which is the sum total of the
// actual size that can be distributed.
//
// do_apply() indicates whether this applies to the given axis.
//
// towards() returns each axis's individual 'share' that contributes to total.
//
// The amount actually applied to each grid position is
// towards() * (share/total). That is, if share is half of total, a half of
// each axis's towards() is applied.

static void prorated_size(const grid_metrics_t &m,
			  const std::vector<grid_xy> &sorted_xy,

			  grid_sizes_t &s,

			  dim_squared_t share,
			  dim_squared_t total,

			  bool (*do_apply)(const axis &),
			  dim_t (*towards)(const axis &))
{
	if (share == 0)
		return; // Edge case

	dim_squared_t carry_over{0};

	for (const auto &xy: sorted_xy)
	{
		const auto &keyvalue=m.at(xy);

		if (do_apply(keyvalue))
		{
			auto numerator=carry_over +
				share * towards(keyvalue);

			std::get<dim_t>(s[xy])
				+= (dim_squared_t::value_type)
				(numerator / total);

			carry_over = numerator % total;
		}
	}
}

#if 0
{
#endif
}
LIBCXXW_NAMESPACE_END
