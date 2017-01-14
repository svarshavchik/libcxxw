/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_new_layoutmanagerobj_h
#define x_w_new_layoutmanagerobj_h

#include <x/w/new_layoutmanagerfwd.H>
#include <x/w/layoutmanagerobj.H>
#include <x/w/containerobj.H>

LIBCXXW_NAMESPACE_START

//! Methods common to all layout managers.

//! \see new_layoutmanager

class new_layoutmanagerObj : virtual public obj {

public:

	//! Construct an internal implementation object.

	virtual ref<layoutmanagerObj::implObj>
	create(const ref<containerObj::implObj> &container_impl) LIBCXX_HIDDEN
		=0;

	//! Constructor
	new_layoutmanagerObj() LIBCXX_HIDDEN;

	//! Destructor
	~new_layoutmanagerObj() LIBCXX_HIDDEN;

};

LIBCXXW_NAMESPACE_END

#endif
