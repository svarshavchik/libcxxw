/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef metrics_grid_grid_H
#define metrics_grid_grid_H

#include <map>
#include <unordered_map>
#include <list>
#include <set>
#include "metrics_grid_pos.H"
#include "metrics_grid_xy.H"
#include "metrics_grid_axisrangefwd.H"
#include "calculate_borders.H"
#include "x/w/metrics/axis.H"
#include <x/number_hash.H>
#include <x/functional.H>

LIBCXXW_NAMESPACE_START

//////////////////////////////////////////////////////////////////////////
//
// Defines calculate_grid_horiz_metrics() and calculate_grid_vert_metrics()
// for effecting grid calculations.

namespace metrics {

	struct LIBCXX_HIDDEN pos_axis_padding {

		dim_t left_padding;

		dim_t top_padding;

		dim_t total_horiz_padding;

		dim_t total_vert_padding;
	};

	//! Callback: given a row or a column number, return its requested size.

	//! The size is specified as a percentage 0-100. A negative value
	//! indicates no specified size.

	typedef int get_req_axis_size_t(grid_xy);

	//! Data for calculations

	//! calculate_grid_horiz_metrics() and calculate_grid_vert_metrics()
	//! process a container of the following values:

	struct LIBCXX_HIDDEN pos_axis {
		//! Horizontal and vertical position of this display element
		grid_pos pos;

		//! Horizontal and vertical metrics of this display element.
		horizvert axises;

		//! The padding to be applied to the axises

		pos_axis_padding padding;
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

		//! The additional padding for this element
		dim_t pos_axis_padding::*axis_padding;

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
			   const axis &a,
			   dim_t axis_padding)
		LIBCXX_HIDDEN;

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
		return calculate_grid_metrics(std::forward<GRID_T>(g),
					      h_grid);
	}

	//! Calculate vertical metrics.

	//! This is called with a container of pos_axis objects.

	template<typename GRID_T> grid_metrics_t
	calculate_grid_vert_metrics(GRID_T &&g)
	{
		return calculate_grid_metrics(std::forward<GRID_T>(g),
					      v_grid);
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
			if (!b->takes_up_space)
				continue;

			grid_axisrange &range=major.get_axisrange(*b);

			grid_by_range[range.end-range.start]
				.push_back(*b);
			all_grid_positions.insert(range.start);
			all_grid_positions.insert(range.end);
		}

		// For grid positions that correspond to borders, prime
		// those metrics to {0,0,0}. This is a preventative measure,
		// preventing those grid positions from having unlimited
		// maximum size.

		if (!all_grid_positions.empty())
		{
			grid_xy last=*--all_grid_positions.end();

			do
			{
				if (IS_BORDER_RESERVED_COORD(last))
				{
					all_grid_positions.insert(last);
					grid_metrics.insert({last, {0,0,0}});
				}
			} while (last-- > 0);
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
					      major.get_axis(element),
					      element.padding
					      .*(major.axis_padding));
			}
		}
		return grid_metrics;
	}

	//! The actual size and position along each grid axis

	typedef std::unordered_map<grid_xy,
				   std::tuple<coord_t, dim_t>> grid_sizes_t;

	//! Compute the actual size of each grid axis position.

	//! Given the target size of a grid's axis, parcel out this size
	//! to each grid position on the axis.
	//!
	//! s are the current sizes. Returns true if the current sizes have
	//! changed.
	//!
	//! The callback returns the requested size of the axis, as a
	//! percentage of the total size (0-100). The callback returns a
	//! negative value if the given axis's size is not specified.

	template<typename callback_t>
	bool calculate_grid_size(const grid_metrics_t &m,
				 grid_sizes_t &s,
				 dim_t target_size,
				 callback_t &&get_req_axis_size)
		LIBCXX_HIDDEN;

	//! Compute the actual size of each grid axis position.

	//! \overload
	//! \see calculate_grid_size()
	bool do_calculate_grid_size(const grid_metrics_t &m,
				    grid_sizes_t &s,
				    dim_t target_size,
				    const function<get_req_axis_size_t>
				    &get_req_axis_size)
		LIBCXX_HIDDEN;

	template<typename callback_t>
	bool calculate_grid_size(const grid_metrics_t &m,
				 grid_sizes_t &s,
				 dim_t target_size,
				 callback_t &&get_req_axis_size)
	{
		return do_calculate_grid_size(m, s, target_size,
					      make_function<get_req_axis_size_t>
					      (std::forward<callback_t>
					       (get_req_axis_size)));
	}
}

LIBCXXW_NAMESPACE_END

#endif
