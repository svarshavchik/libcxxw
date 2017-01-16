/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef gridfactoryobj_h
#define gridfactoryobj_h

#include "x/w/factoryobj.H"
#include "x/w/gridlayoutmanagerfwd.H"
#include "child_elementfwd.H"

LIBCXXW_NAMESPACE_START

//! Implement a factory for the grid layout manager.

//! \see gridfactory

class LIBCXX_HIDDEN gridfactoryObj : public factoryObj {


 public:

	//! This factory is for this layout.
	const gridlayoutmanager gridlayout;

	//! New element's location.
	const dim_t x;

	//! New element's location.
	const dim_t y;

	//! New element's location.
	const dim_t width;

	//! New element's location.
	const dim_t height;

	//! Constructor

	gridfactoryObj(const gridlayoutmanager &gridlayout,
		       dim_t x, dim_t y,
		       dim_t width, dim_t height);

	//! Destructor
	~gridfactoryObj();

	//! Created a new display element for the grid.
	void created(const child_element &new_element) override;
};

LIBCXXW_NAMESPACE_END

#endif
