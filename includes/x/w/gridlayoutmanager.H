/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_gridlayoutmanager_h
#define x_w_gridlayoutmanager_h

#include <x/w/gridlayoutmanagerfwd.H>
#include <x/w/gridlayoutmanagerobj.H>
#include <x/w/new_layoutmanagerfwd.H>
#include <x/ref.H>
#include <x/functionalrefptr.H>

LIBCXXW_NAMESPACE_START

//! Create a new container with a \ref gridlayoutmanager "grid layout manager".

//! This parameter gets passed to a \ref factory "factory"'s
//! \c create_container()
//! when creating a main window, or a container that
//! uses the grid layout manager.

class LIBCXX_HIDDEN new_gridlayoutmanager : public new_layoutmanager {

public:

	//! Constructor
	new_gridlayoutmanager() LIBCXX_PUBLIC;

	//! Destructor
	~new_gridlayoutmanager() LIBCXX_PUBLIC;

	//! Create a container with the grid layout manager.

	//! \internal
	layout_impl create(const container_impl &) const override;
};

LIBCXXW_NAMESPACE_END

#endif
