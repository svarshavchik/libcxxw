/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_new_layoutmanager_h
#define x_w_new_layoutmanager_h

#include <x/w/new_layoutmanagerfwd.H>

LIBCXXW_NAMESPACE_START

//! Create a new grid layout manager.

//! This parameter is passed when create a main window, or a container that
//! uses the grid layout manager.

class LIBCXX_HIDDEN new_gridlayoutmanager : public new_layoutmanager {

public:

	//! Constructor
	new_gridlayoutmanager() LIBCXX_PUBLIC;

	//! Destructor
	~new_gridlayoutmanager() LIBCXX_PUBLIC;

	new_layoutmanager_results create(const new_layoutmanager_info &)
		const override LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
