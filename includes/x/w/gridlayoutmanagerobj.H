/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_gridlayoutmanagerobj_h
#define x_w_gridlayoutmanagerobj_h

#include <x/w/gridlayoutmanagerfwd.H>
#include <x/w/layoutmanagerobj.H>
#include <x/w/factoryfwd.H>
#include <x/w/elementfwd.H>
#include <x/w/types.H>

LIBCXXW_NAMESPACE_START

//! The grid layout manager

//! \see gridlayoutmanager

class gridlayoutmanagerObj : public layoutmanagerObj {

public:
	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	gridlayoutmanagerObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~gridlayoutmanagerObj() LIBCXX_HIDDEN;

	//! Install a new display element in the given location.

	factory insert(dim_t x, dim_t y,
		       dim_t width=1,
		       dim_t height=1);

	//! Remove a display element from the given location.
	void erase(dim_t x, dim_t y);

	//! Get the display element at the given location.
	elementptr get(dim_t x, dim_t y);
};

LIBCXXW_NAMESPACE_END

#endif
