/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef listitemlayoutmanagerfwd_h
#define listitemlayoutmanagerfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class listitemlayoutmanagerObj;

//! The list item layout manager.

//! The list layout manager is just a facade for the grid layout manager.
//! Each row in the grid is a list item. Each element in a grid row is
//! itself a container that uses the listitemlayoutmanager.
//!
//! This layout manager always has one element in the container. Depending
//! on the element in the row this can be either a selected checkbox, or
//! the list item column, or the shortcut/continuation symbol.
//!
//! The listitemlayoutmanager wraps the element in this particular row,
//! adding the prescribed amount of horizontal padding to the element's
//! metrics, and positioning the element in the vertical middle and left
//! alignment. The grid layout manager is set to fill the elements in the
//! grid to the full size of each cell, with no padding. The padding comes
//! from here.
//!
//! As such, the background colors of all elements in each list item's row
//! have no gaps, and the background colors of the individual items in each
//! row effectively manage to seamlessly highlight each row in the list.

typedef ref<listitemlayoutmanagerObj> listitemlayoutmanager;

LIBCXXW_NAMESPACE_END

#endif
