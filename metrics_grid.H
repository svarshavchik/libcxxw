/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef metrics_grid_grid_H
#define metrics_grid_grid_H

#include <map>
#include <list>
#include <set>
#include "metrics_grid_pos.H"
#include "metrics_grid_xy.H"
#include "metrics_grid_axisrangefwd.H"
#include "x/w/metrics/axis.H"

LIBCXXW_NAMESPACE_START

//////////////////////////////////////////////////////////////////////////
//
// Defines calculate_grid_horiz_metrics() and calculate_grid_vert_metrics()
// for effecting grid calculations.

namespace metrics {

	//! Data for calculations

	//! calculate_grid_horiz_metrics() and calculate_grid_vert_metrics()
	//! process a container of the following values:

	struct pos_axis {
		//! Horizontal and vertical position of this display element
		grid_pos pos;

		//! Horizontal and vertical metrics of this display element.
		horizvert axises;
	};

	//! Computed horizontal and vertical grid metrics.

	//! Based on the metrics given in a grid, computed metrics of
	//! rows or columns.

	typedef std::map<grid_xy, axis> grid_metrics_t;

	//! What grid metric calculations look at.

	//! Both horizontal and vertical metrics use the same logic.
	//! This tells the code whether we're looking at grid_pos's, and
	//! the associated element metric rectangle's horizontal or vertical
	//! axises.

	struct LIBCXX_INTERNAL grid_major {
		//! The horizontal or the vertical grid element's position.
		grid_axisrange grid_posObj::*grid_pos_member;

		//! The horizontal or the vertical grid element's metrics axis.
		axis horizvertObj::*axis_pos_member;

		//! The other metric calculations.
		const struct grid_major *minor;

		//! Convenient macro
		inline grid_axisrange &get_axisrange(const pos_axis &p) const
		{
			return (*p.pos).*grid_pos_member;
		}

		//! Convenient macro
		inline axis &get_axis(const pos_axis &p) const
		{
			return (*p.axises).*axis_pos_member;
		}
	};

	extern const struct grid_major h_grid;
	extern const struct grid_major v_grid;

	//! Internal calculation function invoked from calculate_grid_metrics().

	//! Does not need to be exposed in the header file.
	void apply_metrics(grid_metrics_t &m,
			   const std::set<grid_xy> &all_grid_positions,
			   const grid_axisrange &r,
			   const axis &a) LIBCXX_HIDDEN;

	// Visibility declarations.

	template<typename GRID_T> grid_metrics_t
	calculate_grid_horiz_metrics(GRID_T &&g) LIBCXX_HIDDEN;

	template<typename GRID_T> grid_metrics_t
	calculate_grid_vert_metrics(GRID_T &&g) LIBCXX_HIDDEN;

	template<typename GRID_T> grid_metrics_t
	calculate_grid_metrics(GRID_T &&g,
			       const grid_major &major) LIBCXX_HIDDEN;

	//! Calculate horizontal metrics.

	//! This is called with a container of pos_axis objects.

	template<typename GRID_T> grid_metrics_t
	calculate_grid_horiz_metrics(GRID_T &&g)
	{
		return calculate_grid_metrics(std::forward<GRID_T>(g), h_grid);
	}

	//! Calculate vertical metrics.

	//! This is called with a container of pos_axis objects.

	template<typename GRID_T> grid_metrics_t
	calculate_grid_vert_metrics(GRID_T &&g)
	{
		return calculate_grid_metrics(std::forward<GRID_T>(g), v_grid);
	}

	//! Calculate horizontal  or vertical metrics.

	template<typename GRID_T> grid_metrics_t
	calculate_grid_metrics(GRID_T &&g,
			       const grid_major &major)
	{
		grid_metrics_t grid_metrics;

		typedef std::map<grid_xy, std::list<pos_axis>
				 > grid_by_range_map;

		// Sort grid elements by their axis's range.

		// grid_by_range[0] is a list of all elements whose
		// start=end.

		// grid_by_range[1]  is a list of all elements whose
		// start+1=end.
		// And so on.

		grid_by_range_map grid_by_range;

		std::set<grid_xy> all_grid_positions;

		for (auto b=g.begin(), e=g.end(); b != e; ++b)
		{
			grid_axisrange &range=major.get_axisrange(*b);

			grid_by_range[range.end-range.start]
				.push_back(*b);
			all_grid_positions.insert(range.start);
			all_grid_positions.insert(range.end);
		}

		// Now we iterate over everything, in the right order,
		// and factor in each cell's metrics.
		//
		// We start with the elements that span the smallest
		// range, then work our way up to the bigger ones.

		for (const auto &range:grid_by_range)
		{
			for (const auto &element:range.second)
			{
				apply_metrics(grid_metrics,
					      all_grid_positions,
					      major.get_axisrange
					      (element),
					      major.get_axis(element));
			}
		}
		return grid_metrics;
	}

	//! The actual size of a grid's axis.

	//! This is a vector whose size is always the same as the number
	//! of keys in grid_metrics_t.

	typedef std::vector<dim_t> grid_sizes_t;

	//! Compute the actual size of each grid axis position.

	//! Given the target size of a grid's axis, parcel out this size
	//! to each grid position on the axis.

	void calculate_grid_size(const grid_metrics_t &m,
				 grid_sizes_t &s,
				 dim_t target_size) LIBCXX_HIDDEN;
}

LIBCXXW_NAMESPACE_END

#endif
