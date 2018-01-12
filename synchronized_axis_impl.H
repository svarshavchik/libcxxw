/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef synchronized_axis_impl_h
#define synchronized_axis_impl_h

#include "x/w/synchronized_axis.H"
#include "synchronized_axis_valuefwd.H"

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

class LIBCXX_HIDDEN synchronized_axisObj::implObj : virtual public obj {

 public:
	//! Constructor
	implObj();

	//! Destructor
	~implObj();

	//! All axis metrics that get synchronized.

	//! Together with the cached derived axis metrics.
	synchronized_values values;
};

LIBCXXW_NAMESPACE_END

#endif
