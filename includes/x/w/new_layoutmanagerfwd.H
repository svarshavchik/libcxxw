/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_new_layoutmanagerfwd_h
#define x_w_new_layoutmanagerfwd_h

#include <x/w/namespace.H>

LIBCXXW_NAMESPACE_START

class new_layoutmanager_info;
class new_layoutmanager_results;

//! A new layout manager

//! A new container's layout manager gets created before the
//! \ref container "container" gets created.
//!
//! A subclass of new_layoutmanager gets passed to methods that create
//! layout managers.

class LIBCXX_HIDDEN new_layoutmanager {

public:

	new_layoutmanager();

	~new_layoutmanager();

	virtual new_layoutmanager_results create(const new_layoutmanager_info &)
		const =0;
};

LIBCXXW_NAMESPACE_END

#endif
