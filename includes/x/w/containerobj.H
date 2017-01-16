/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_containerobj_h
#define x_w_containerobj_h

#include <x/w/containerfwd.H>
#include <x/w/layoutmanagerfwd.H>
#include <x/w/layoutmanagerobj.H>
#include <x/w/new_layoutmanagerfwd.H>
#include <x/w/elementobj.H>

LIBCXXW_NAMESPACE_START

//! Methods common to all display containers.

//! \see container

class containerObj : public elementObj {

public:
	class implObj;

	//! Internal implementation object.

	const ref<implObj> impl;

	//! Layout manager's internal implementation object.

	const ref<layoutmanagerObj::implObj> layout_impl;

	//! Return this container's layout manager.
	layoutmanager get_layoutmanager();

	//! Return this container's layout manager.
	const_layoutmanager get_layoutmanager() const;

	//! Constructor
	containerObj(const ref<implObj> &impl,
		     const new_layoutmanager &layout_factory) LIBCXX_HIDDEN;

	//! Destructor
	~containerObj() LIBCXX_HIDDEN;

};

LIBCXXW_NAMESPACE_END

#endif
