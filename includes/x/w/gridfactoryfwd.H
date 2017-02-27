/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_gridfactoryfwd_h
#define x_w_gridfactoryfwd_h

#include <x/ptrfwd.H>
#include "x/w/namespace.H"

LIBCXXW_NAMESPACE_START

class gridfactoryObj;

//! Factory implementation for a grid.

//! This grid factory gets returned by the grid layout manager. Most of the
//! methods that create new display elements in the grid are inherited
//! from \ref factory "the factory base class". Each call to
//! the layout manager's methods add the new element to the grid, one after
//! another on the same row.
//!
//! The \ref gridlayoutmanager "grid layout manager's" append_row() method
//! returns a grid factory that initially adds new elements to a new row that's
//! added to the bottom of the grid.
//!
//! \code
//! INSERT_LIBX_NAMESPACE::w::border_infomm new_border;
//!
//! gridfactory->border(new_border);
//! gridfactory->left_border(new_border);
//! gridfactory->right_border(new_border);
//! gridfactory->top_border(new_border);
//! gridfactory->bottom_border(new_border);
//!
//! gridfactory->padding(2.5);
//! gridfactory->left_padding(2.5);
//! gridfactory->right_padding(2.5);
//! gridfactory->top_padding(2.5);
//! gridfactory->bottom_padding(2.5);
//! \endcode
//!
//! Each display element in the grid has borders and inner padding.
//! Before creating and adding a new display element to the grid, these
//! methods specify its border and the padding.
//!
//! border() and padding() set the border and the padding for all four sides,
//! or they can be set individually.
//!
//! A border gets specified as a \ref border_infomm "INSERT_LIBX_NAMESPACE::w::border_infomm".
//! The padding gets specified in millimeters.

typedef ref<gridfactoryObj> gridfactory;

LIBCXXW_NAMESPACE_END

#endif
