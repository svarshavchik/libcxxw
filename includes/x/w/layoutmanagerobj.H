/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_layoutmanagerobj_h
#define x_w_layoutmanagerobj_h

#include <x/w/layoutmanagerfwd.H>
#include <x/w/batch_queuefwd.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Methods common to all layout managers.

//! \see layoutmanager

class layoutmanagerObj : virtual public obj {

public:
	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Internal implementation object.

	const batch_queue queue;

	//! Constructor
	layoutmanagerObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~layoutmanagerObj() LIBCXX_HIDDEN;

};

LIBCXXW_NAMESPACE_END

#endif
