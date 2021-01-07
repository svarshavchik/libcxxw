/*
** Copyright 2018-2021 Double Precision, Inc.
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

	//! This element's native metrics.

	std::vector<metrics::axis> values_thread_only;

	//! The actual size of this element, at the moment.

	//! It's expected that the synchronized elements' container will
	//! set them to the same size. That's the only way this'll work.
	//! But each synchronized element can only vouch for itself.
	dim_t minimum_thread_only;

	//! This element's column's requested width percentages.

	//! It's expected that all synchronized elements specify the
	//! same requested column widths, but each synchronized element
	//! can only vouch for itself, and the derived value calculation
	//! will have to pick one of the elements as a reference.

	std::unordered_map<size_t, int> requested_col_widths_thread_only;
public:
	THREAD_DATA_ONLY(values);
	THREAD_DATA_ONLY(minimum);
	THREAD_DATA_ONLY(requested_col_widths);

	//! Constructor
	synchronized_axis_valueObj();

	//! Destructor
	~synchronized_axis_valueObj();

	//! New synchronized values have been computed.

	//! Each element inherits and overrides synchronized_axis_updated()
	//! in order to be notified when the synchronized axis values change.
	virtual void synchronized_axis_updated
		(ONLY IN_THREAD,
		 const synchronized_axis_values_t &)=0;
};

LIBCXXW_NAMESPACE_END

#endif
