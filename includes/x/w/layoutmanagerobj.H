/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_layoutmanagerobj_h
#define x_w_layoutmanagerobj_h

#include <x/w/layoutmanagerfwd.H>
#include <x/w/new_layoutmanagerfwd.H>
#include <x/w/elementobj.H>
#include <x/w/containerobj.H>

LIBCXXW_NAMESPACE_START

//! Methods common to all layout managers.

//! \see layoutmanager

class layoutmanagerObj : virtual public obj {

public:
	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	layoutmanagerObj(const new_layoutmanager &layout_factory,
			 const ref<containerObj::implObj> &container_impl)
		LIBCXX_HIDDEN;

	//! Destructor
	~layoutmanagerObj() LIBCXX_HIDDEN;

};

LIBCXXW_NAMESPACE_END

#endif
