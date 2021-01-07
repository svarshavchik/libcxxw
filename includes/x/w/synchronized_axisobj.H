/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_synchronized_axisobj_h
#define x_w_synchronized_axisobj_h

#include <x/w/synchronized_axisfwd.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see synchronized_axis

class synchronized_axisObj : virtual public obj {

public:
	class LIBCXX_HIDDEN implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	synchronized_axisObj();

	//! Constructor
	synchronized_axisObj(const ref<implObj> &impl);

	//! Destructor
	~synchronized_axisObj();
};

LIBCXXW_NAMESPACE_END

#endif
