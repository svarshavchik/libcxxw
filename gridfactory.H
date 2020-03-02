/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef gridfactory_h
#define gridfactory_h

#include "x/w/gridfactory.H"
#include "x/w/border_infomm.H"
#include "x/w/impl/current_border_implfwd.H"
#include "grid_elementobj.H"
#include <x/mpobj.H>

LIBCXXW_NAMESPACE_START

//! Grid factory implementation object.

//! \see gridfactory

class LIBCXX_HIDDEN gridfactoryObj::implObj : virtual public obj {


 public:

	//! new_grid_element_info doesn't really need to be mutex-protected.

	typedef mpobj<new_grid_element_info> new_grid_element_t;

	//! Collected information about the new grid element.

	new_grid_element_t new_grid_element;

	//! Whether we are replacing an existing element.
	const bool replacing;

	//! Constructor
	implObj(dim_t row, dim_t col, const ref<grid_map_infoObj> &grid_map,
		const container_impl &parent_container,
		bool replacing);

	//! Destructor
	~implObj();
};

LIBCXXW_NAMESPACE_END

#endif
