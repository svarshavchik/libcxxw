/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_toolboxlayoutmanager_h
#define x_w_toolboxlayoutmanager_h

#include <x/w/toolboxlayoutmanagerfwd.H>
#include <x/w/toolboxlayoutmanagerobj.H>
#include <x/w/layoutmanager.H>
#include <x/w/new_layoutmanagerfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC new_toolboxlayoutmanager;

//! Create a container that uses the toolbox layout manager.

class new_toolboxlayoutmanager : public new_layoutmanager {

public:
	//! Constructor
	new_toolboxlayoutmanager() LIBCXX_PUBLIC;

	//! Destructor
	~new_toolboxlayoutmanager() LIBCXX_PUBLIC;

	//! Initial number of columns in the toolbox
	size_t default_width=2;

	//! Create a container with the toolbox layout manager.

	//! \internal
	layout_impl create(const container_impl &) const override LIBCXX_PUBLIC;
};

LIBCXXW_NAMESPACE_END

#endif
