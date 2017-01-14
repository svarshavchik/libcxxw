/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_new_layoutmanager_h
#define x_w_new_layoutmanager_h

#include <x/w/new_layoutmanagerfwd.H>
#include <x/w/new_layoutmanagerobj.H>
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

//! Base class for new layout managers

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::new_layoutmanager::base

class new_layoutmanagerBase {

public:

	static new_layoutmanager create_grid();
};

LIBCXXW_NAMESPACE_END

#endif
