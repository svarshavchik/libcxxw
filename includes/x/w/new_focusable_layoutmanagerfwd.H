/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_new_focusable_layoutmanagerfwd_h
#define x_w_new_focusable_layoutmanagerfwd_h

#include <x/w/namespace.H>
#include <x/w/focusable_containerfwd.H>
#include <x/w/focusable_containerobj.H>

#include <x/functional.H>

LIBCXXW_NAMESPACE_START

//! A new focusable layout manager

//! A new focusable container's layout manager gets created before the
//! \ref focusable_container "focusable_container" gets created.
//!
//! A subclass of new_focusable_layoutmanager gets passed to methods that create
//! focusable layout managers.

class LIBCXX_HIDDEN new_focusable_layoutmanager {

public:

	//! Constructor
	new_focusable_layoutmanager();

	//! Destructor
	~new_focusable_layoutmanager();

	//! Internal implementation
	virtual focusable_container create(const ref<containerObj::implObj> &)
		const=0;
};

LIBCXXW_NAMESPACE_END

#endif
