/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_new_layoutmanagerfwd_h
#define x_w_new_layoutmanagerfwd_h

#include <x/w/namespace.H>
#include <x/w/containerfwd.H>
#include <x/w/containerobj.H>

#include <x/functional.H>

LIBCXXW_NAMESPACE_START

//! A new layout manager

//! A new container's layout manager gets created before the
//! \ref container "container" gets created.
//!
//! A subclass of new_layoutmanager gets passed to methods that create
//! layout managers.

class LIBCXX_HIDDEN new_layoutmanager {

public:

	//! Constructor
	new_layoutmanager();

	//! Destructor
	~new_layoutmanager();

	//! Internal implementation
	virtual ref<layoutmanagerObj::implObj>
		create(const ref<containerObj::implObj> &) const=0;

	//! Internal implementation
	virtual container create(const ref<containerObj::implObj> &,
				 const function<void(const container &)> &)
		const;
};

LIBCXXW_NAMESPACE_END

#endif
