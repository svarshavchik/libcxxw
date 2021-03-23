/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef grid_elementobj_h
#define grid_elementobj_h

#include "x/w/impl/grid_elementfwd.H"
#include "x/w/impl/current_border_implfwd.H"
#include "x/w/impl/themedim_elementfwd.H"
#include "metrics_grid_posfwd.H"
#include "metrics_grid.H"
#include "x/w/defaultthemefwd.H"
#include "x/w/elementfwd.H"
#include "x/w/screenfwd.H"
#include "x/w/types.H"
#include "x/w/grid_map_infofwd.H"
#include "x/w/dim_arg.H"
#include "x/w/containerobj.H"
#include "x/w/screenobj.H"

#include <x/obj.H>

LIBCXXW_NAMESPACE_START

struct LIBCXX_HIDDEN new_grid_element_info;
struct LIBCXX_HIDDEN existing_grid_element_info;

//! Info about the new grid element

//! A \ref grid_element is just a reference-counted subclass of this.

struct existing_grid_element_info {

	//! New element's location.
	dim_t width=1;

	//! New element's location.
	dim_t height=1;

	//! New element's border
	current_border_implptr left_border;

	//! New element's border
	current_border_implptr right_border;

	//! New element's border
	current_border_implptr top_border;

	//! New element's border
	current_border_implptr bottom_border;

	//! Horizontal alignment
	std::optional<halign> horizontal_alignment;

	//! Vertical alignment
	valign vertical_alignment=valign::top;

	//! When this display element is hidden, "remove" it from the grid.

	//! It is not actually removed, just ignored for the purposes of
	//! calculating the grid layout.

	bool remove_when_hidden=false;
};

struct new_grid_element_info : public existing_grid_element_info {

	//! Padding
	dim_arg left_padding_set;

	//! Padding
	dim_arg right_padding_set;

	//! Padding
	dim_arg top_padding_set;

	//! Padding
	dim_arg bottom_padding_set;

	//! New element's location
	dim_t row;

	//! New element's location
	dim_t col;

	//! My screen
	ref<screenObj::implObj> screen_impl;

	//! Constructor
	new_grid_element_info(dim_t row,
			      dim_t col,
			      const container_impl &parent_container,
			      const ref<grid_map_infoObj> &grid_map);

	//! Copy constructor
	new_grid_element_info(const new_grid_element_info &);

	//! Destructor
	~new_grid_element_info();
};

struct LIBCXX_HIDDEN left_padding_tag;
struct LIBCXX_HIDDEN right_padding_tag;
struct LIBCXX_HIDDEN top_padding_tag;
struct LIBCXX_HIDDEN bottom_padding_tag;

//! A \ref grid_element object.

class grid_elementObj
	: public existing_grid_element_info,
	  public metrics::pos_axis_padding,
	  public themedim_element<left_padding_tag>,
	  public themedim_element<right_padding_tag>,
	  public themedim_element<top_padding_tag>,
	  public themedim_element<bottom_padding_tag>,
	  virtual public obj {

 public:
	THREAD_DATA_ONLY(initialized);

	//! Constructor
	grid_elementObj(const new_grid_element_info &info,
			const element &grid_element);

	//! Destructor
	~grid_elementObj();

	//! Initialize this grid element in the connection thread.
	void initialize(ONLY IN_THREAD,
			const ref<screenObj::implObj> &screen_impl);

	//! The element
	const element grid_element;

	//! Computed by calculate_borders()

	const metrics::grid_pos pos;

 private:
	//! Whether this grid element has been initialized in the connection thread.
	bool initialized_thread_only;

 private:
	//! Calculate this grid element's padding.

	//! Converts _paddingmm to pixels. Sets left_padding and
	//! top_padding, and horiz_total_padding and vert_total_padding
	//! in grid_element picking up the slack (the grid calculations look
	//! at grid_element's axises, and the only thing that the grid
	//! calculation cares about is the total padding in each axis).
	void recalculate_padding(ONLY IN_THREAD);

public:
	//! Call recalculate_padding().

	void theme_updated(ONLY IN_THREAD,
			   const const_defaulttheme &new_theme);

	//! Return true if the element is visible, or if its space is reserved.

	bool takes_up_space(ONLY IN_THREAD) const;
};

LIBCXXW_NAMESPACE_END

#endif
