/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_gridlayoutmanager_h
#define x_w_gridlayoutmanager_h

#include <x/w/gridlayoutmanagerfwd.H>
#include <x/w/gridlayoutmanagerobj.H>
#include <x/w/new_layoutmanager.H>
#include <x/w/synchronized_axisfwd.H>
#include <x/w/dim_arg.H>
#include <x/ref.H>
#include <x/functionalrefptr.H>

LIBCXXW_NAMESPACE_START

//! Create a new container with a \ref gridlayoutmanager "grid layout manager".

//! This parameter gets passed to a \ref factory "factory"'s
//! \c create_container()
//! when creating a main window, or a container that
//! uses the grid layout manager.

class new_gridlayoutmanager : public new_layoutmanager {

public:

	//! Constructor
	new_gridlayoutmanager() LIBCXX_PUBLIC;

	//! Destructor
	~new_gridlayoutmanager() LIBCXX_PUBLIC;

	//! Copy constructor
	new_gridlayoutmanager(const new_gridlayoutmanager &) LIBCXX_PUBLIC;

	//! Move constructor
	new_gridlayoutmanager(new_gridlayoutmanager &&) LIBCXX_PUBLIC;

	//! Assignment operator
	new_gridlayoutmanager &operator=(const new_gridlayoutmanager &)
		LIBCXX_PUBLIC;

	//! Move operator
	new_gridlayoutmanager &operator=(new_gridlayoutmanager &&)
		LIBCXX_PUBLIC;

	//! Default horizontal padding of new cells.
	dim_arg col_padding{"grid_horiz_padding"};

	//! Default vertical padding of new cells.
	dim_arg row_padding{"grid_vert_padding"};

	//! Defau
	//! What other grid this grid's columns get synchronized with.

	//! This is mostly for internal use. Using the same
	//! new_gridlayoutmanager to create multiple containers results
	//! in columns in both containers always having the same width.
	//!
	//! For this to work correctly, both grids should have the
	//! same number of columns, AND the same vertical borders.
	//!
	//! It is also possible to, very carefully, synchronize the columns
	//! between a grid and the list, see
	//! \ref new_listlayoutmanager "INSERT_LIBX_NAMESPACE::w::new_listlayoutmanager"
	//! for more information.

	synchronized_axis synchronized_columns;

	//! Create a container with the grid layout manager.

	//! \internal
	layout_impl create(const container_impl &) const override LIBCXX_PUBLIC;
};

LIBCXXW_NAMESPACE_END

#endif
