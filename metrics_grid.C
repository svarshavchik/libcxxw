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

//#define DEBUG

#ifdef DEBUG
#include <iostream>
#endif

LIBCXXW_NAMESPACE_START
namespace metrics {
#if 0
}
#endif

//! What grid metric calculations look at.

//! Both horizontal and vertical metrics use the same logic.
//! This tells the code whether we're looking at grid_pos's, and
//! the associated element metric rectangle's horizontal or vertical
//! axises.

struct LIBCXX_INTERNAL grid_major {
	//! The horizontal or the vertical grid element's position.
	grid_axisrange grid_posObj::*grid_pos_member;

	//! The horizontal or the vertical grid element's metrics axis.
	axis rectangle::*axis_pos_member;

	//! The other metric calculations.
	const struct grid_major *minor;

	//! Convenient macro
	inline grid_axisrange &get_axisrange(const grid_pos &p) const
	{
		return (*p).*grid_pos_member;
	}

	//! Convenient macro
	inline axis &get_axis(const grid_pos &p) const
	{
		return (*p->metrics).*axis_pos_member;
	}
};

extern const struct grid_major h_grid;
extern const struct grid_major v_grid;

const struct grid_major h_grid LIBCXX_INTERNAL ={
	&grid_posObj::horiz_pos,
	&rectangle::horiz,
	&v_grid,
};

const struct grid_major v_grid LIBCXX_INTERNAL ={
	&grid_posObj::vert_pos,
	&rectangle::vert,
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

static void apply_metrics(grid_metrics_t &m,
			  const std::set<grid_xy> &all_grid_positions,
			  const grid_axisrange &r,
			  const axis &a) LIBCXX_INTERNAL;

static grid_metrics_t calculate_grid_metrics(const grid_t &g,
					     const grid_major &major)
	LIBCXX_INTERNAL;

grid_metrics_t calculate_grid_horiz_metrics(const grid_t &g)
{
	return calculate_grid_metrics(g, h_grid);
}

grid_metrics_t calculate_grid_vert_metrics(const grid_t &g)
{
	return calculate_grid_metrics(g, v_grid);
}

static grid_metrics_t calculate_grid_metrics(const grid_t &g,
					     const grid_major &major)
{
	grid_metrics_t grid_metrics;

	typedef std::map<grid_xy, std::list<grid_pos>> grid_by_range_map;

	// Sort grid elements by their axis's range.

	// grid_by_range[0] is a list of all elements whose start=end.
	// grid_by_range[1]  is a list of all elements whose start+1=end.
	// And so on.

	grid_by_range_map grid_by_range;

	std::set<grid_xy> all_grid_positions;

	for (const auto &element:g)
	{
		grid_axisrange &range=major.get_axisrange(element);

		grid_by_range[range.end-range.start].push_back(element);
		all_grid_positions.insert(range.start);
		all_grid_positions.insert(range.end);
	}
#ifdef DEBUG
	std::cout << std::endl;
	std::for_each(all_grid_positions.begin(),
		      all_grid_positions.end(),
		      []
		      (const auto &p)
		      {
			      std::cout << "Position: " << p << std::endl;
		      });
#endif

	// Now we iterate over everything, in the right order, and factor in
	// each cell's metrics.
	//
	// We start with the elements that span the smallest range, then
	// work our way up to the bigger ones.
	for (const auto &range:grid_by_range)
	{
		for (const auto &element:range.second)
		{
			apply_metrics(grid_metrics,
				      all_grid_positions,
				      major.get_axisrange(element),
				      major.get_axis(element));
		}
	}
	return grid_metrics;
}

// Ok, we want to factor in a grid element that spans the given range,
// and has the given metrics.

static void apply_metrics(grid_metrics_t &m,
			  const std::set<grid_xy> &all_grid_positions,
			  const grid_axisrange &r,
			  const axis &a)
{
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
			total_minimum < (dim_t::value_type)a.minimum()
					? a.minimum()-total_minimum
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

		if (total_preferred < (dim_t::value_type)a.preferred())
		{
			new_preferred=a.preferred()-total_preferred;
			if (new_preferred < new_minimum)
				new_preferred=new_minimum;
		}

		// Opening bid: the maximum size is infinite.
		dim_t new_maximum=dim_t::infinite();

		// But if it's not:
		if (a.maximum() != dim_t::infinite())
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
			    < (dim_t::value_type)a.maximum())
			{
				new_maximum=a.maximum() - total_maximum.sum_excluding_infinite;
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
	if (total_minimum < (dim_t::value_type)a.minimum())
	{
		axis::adjust_minimums_by(v.begin(), v.end(),
					 (dim_t::value_type)a.minimum()
					 -total_minimum);

		// adjust_minimums_by() could've updated some position's
		// maximum value too, so recalculate the total_maximum.
		total_maximum=axis::total_maximum(v.begin(), v.end());
	}

	// If the new element's maximum size is not infinite, we need to
	// enforce it.
	if (a.maximum() != dim_t::infinite())
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
		    > (dim_t::value_type)a.maximum())
		{
			axis::adjust_maximums_by(v.begin(), v.end(),
						 total_maximum
						 .sum_excluding_infinite -
						 (dim_t::value_type)a.maximum()
						 );
		}
	}
}


#if 0
{
#endif
}
LIBCXXW_NAMESPACE_END
