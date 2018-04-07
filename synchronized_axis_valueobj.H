/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_synchronized_axis_valueobj_h
#define x_w_synchronized_axis_valueobj_h

#include "synchronized_axis_valuefwd.H"
#include "metrics_axis.H"
#include <list>
#include <vector>
#include "x/w/impl/connection_threadfwd.H"

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see synchronized_axis_value

class LIBCXX_HIDDEN synchronized_axis_valueObj : virtual public obj {

	//! The values being synchronized

	std::vector<metrics::axis> values_thread_only;
public:
	THREAD_DATA_ONLY(values);

	//! Constructor
	synchronized_axis_valueObj();

	//! Destructor
	~synchronized_axis_valueObj();

	//! New synchronized values have been computed.

	virtual void updated(ONLY IN_THREAD,
			     const std::vector<metrics::derivedaxis> &)=0;
};

LIBCXXW_NAMESPACE_END

#endif
