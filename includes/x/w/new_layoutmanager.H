/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_new_layoutmanager_h
#define x_w_new_layoutmanager_h

#include <x/w/namespace.H>
#include <x/w/containerfwd.H>
#include <x/w/containerobj.H>
#include <x/w/new_layoutmanagerfwd.H>

#include <x/functional.H>

LIBCXXW_NAMESPACE_START

//! A new layout manager

//! A new container's layout manager gets created before the
//! \ref container "container" gets created.
//!
//! A subclass of new_layoutmanager gets passed to methods that create
//! layout managers.

class new_layoutmanager {

public:

	//! Constructor
	new_layoutmanager();

	//! Destructor
	~new_layoutmanager();

	//! Internal implementation
	virtual layout_impl create(const container_impl &) const LIBCXX_PUBLIC=0;

	//! Internal implementation
	virtual container create(const container_impl &,
				 const function<void(const container &)> &)
		const LIBCXX_PUBLIC;
};

LIBCXXW_NAMESPACE_END

#endif
