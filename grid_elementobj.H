/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef grid_elementobj_h
#define grid_elementobj_h

#include "x/w/grid_elementfwd.H"
#include "current_border_implfwd.H"
#include "metrics_grid_posfwd.H"
#include "x/w/elementfwd.H"
#include "x/w/types.H"

#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Info about the new grid element

//! A \ref grid_element is just a reference-counted subclass of this.

struct new_grid_element_info {

	//! New element's location
	dim_t row;

	//! New element's location
	dim_t col;

	//! New element's location.
	dim_t width=1;

	//! New element's location.
	dim_t height=1;

	//! New element's border
	current_border_impl left_border;

	//! New element's border
	current_border_impl right_border;

	//! New element's border
	current_border_impl top_border;

	//! New element's border
	current_border_impl bottom_border;

	//! Constructor
	new_grid_element_info(dim_t row,
			      dim_t col,
			      const current_border_impl &initial_border);

	//! Copy constructor
	new_grid_element_info(const new_grid_element_info &);

	//! Destructor
	~new_grid_element_info();
};

//! A \ref grid_element object.

class LIBCXX_HIDDEN grid_elementObj
	: public new_grid_element_info, virtual public obj {

 public:
	//! Constructor
	grid_elementObj(const new_grid_element_info &info,
			const element &grid_element);

	//! Destructor
	~grid_elementObj();

	//! The element
	const element grid_element;

	//! Computed by calculate_borders()

	const metrics::grid_pos pos;
};

LIBCXXW_NAMESPACE_END

#endif
