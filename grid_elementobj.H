/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef grid_elementobj_h
#define grid_elementobj_h

#include "grid_elementfwd.H"
#include "current_border_implfwd.H"
#include "metrics_grid_posfwd.H"
#include "metrics_grid.H"
#include "defaultthemefwd.H"
#include "x/w/elementfwd.H"
#include "x/w/screenfwd.H"
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

	//! Padding
	double left_paddingmm=.5;

	//! Padding
	double right_paddingmm=.5;

	//! Padding
	double top_paddingmm=.5;

	//! Padding
	double bottom_paddingmm=.5;

	//! Horizontal alignment
	halign horizontal_alignment=halign::left;

	//! Vertical alignment
	valign vertical_alignment=valign::top;

	//! Constructor
	new_grid_element_info(dim_t row,
			      dim_t col,
			      const current_border_impl &initial_border);

	//! Copy constructor
	new_grid_element_info(const new_grid_element_info &);

	//! Destructor
	~new_grid_element_info();
};

//! Lock acquired for calculating pixel padding dimensions.

struct LIBCXX_HIDDEN grid_element_padding_lock {

	//! Element's screen
	const screen my_screen;

	//! Acquired theme lock on the screen.
	current_theme_t::lock theme_lock;

	//! Constructor
	grid_element_padding_lock(const screen &my_screen);

	//! Destructor
	~grid_element_padding_lock();
};

//! A \ref grid_element object.

class LIBCXX_HIDDEN grid_elementObj
	: public new_grid_element_info,
	  public metrics::pos_axis_padding,
	  virtual public obj {

 public:
	//! Constructor
	grid_elementObj(const new_grid_element_info &info,
			const element &grid_element,
			const grid_element_padding_lock &lock);

	//! Destructor
	~grid_elementObj();

	//! The element
	const element grid_element;

	//! Computed by calculate_borders()

	const metrics::grid_pos pos;

	//! Calculate this grid element's padding.

	//! Converts _paddingmm to pixels. Sets left_padding and
	//! top_padding, and horiz_total_padding and vert_total_padding
	//! in grid_element picking up the slack (the grid calculations look
	//! at grid_element's axises, and the only thing that the grid
	//! calculation cares about is the total padding in each axis).

	void calculate_padding(const grid_element_padding_lock &lock);
};

LIBCXXW_NAMESPACE_END

#endif
