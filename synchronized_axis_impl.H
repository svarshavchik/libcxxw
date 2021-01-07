/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef synchronized_axis_impl_h
#define synchronized_axis_impl_h

#include "x/w/synchronized_axis.H"
#include "synchronized_axis_valuefwd.H"

LIBCXXW_NAMESPACE_START

struct LIBCXX_HIDDEN synchronized_axis_values_t {

	//! My object.

	//! synchronized_axis_values_t is instantiated as a member of the
        //! synchronized_axisObj::implObj class. This is a reference to an
        //! instance of the class that this synchronized_axis_values_t is
        //! an instance of.

        synchronized_axisObj::implObj &me;

	//! Constructor
	synchronized_axis_values_t(synchronized_axisObj::implObj &me);

	//! List of all the values being synchronized.

	std::list<synchronized_axis_value> all_values;

	//! The computed synchronized set of values.

	//! For intermediate use only. The unscaled values should be used to
	//! compute the metrics of the display element.
	std::vector<metrics::derivedaxis> unscaled_values;

	//! The derived_values, scaled to the largest minimum size.

	//! This should be used by the display element for drawing its
	//! contents. Typically, the synchronized elements compute their
	//! metrics, and dump them into their values. Then we compute
	//! unscaled_values based on them, and all display elements use them
	//! to set their final metrics. Arrangements are then made by the
	//! elements' container to set their total size to at least their
	//! final metrics, but it could be more than that.
	//!
	//! We expect that all elements end up having the same minimum and
	//! requested widths. But if not we pick the largest minimum, then
	//! use that element's requested column widths to compute the
	//! scaled values that all synchronized elements use to draw themselves.
	std::vector<metrics::axis> scaled_values;

	//! Recalculate the derived_values.

	//! Invokes all_values' updated(), except the ones whose iterator
	//! gets passed into here. This is the value that was changed, and
	//! the values' element is already responsible for recalculating it,
	//! so no need to do it again.

	void recalculate(ONLY IN_THREAD,
			 std::list<synchronized_axis_value>::iterator
			 iter);

	//! Notify all synchronized axises, that we were updated.

	//! Invoked by the table layout manager
	//! when it manually adjusts the scaled_values
	void notify(ONLY IN_THREAD);

	//! Notify all synchronized axises, that we were updated.

	//! Invoked by recalculate().
	void notify(ONLY IN_THREAD,
		    std::list<synchronized_axis_value>::iterator);
};

//! Acquire a lock for the purpose of updating the synchronized values.

class my_synchronized_axis::lock : public synchronized_values::lock {

	//! My parent object.

	my_synchronized_axis &me;

public:
	//! Constructor
	lock(my_synchronized_axis &);

	//! Destructor
	~lock();

	using synchronized_values::lock::operator->;

	//! Whether there are any other values that will be synchronized with.

	//! Allows the caller to avoid doing hard work.
	bool has_synchronized_values(ONLY IN_THREAD);

	//! After updating my_value, recalculate the synchronized axis.

	//! Pass in the new values to be synchronized with all others.
	//! Compares the passed-in values to my_value->values,
	//! and does nothing if they're the same. Otherwise:
	//!
	//! Invoke all updated(), except for this element's my_value's
	//! updated() callback. It is expected that synchronize()'s
	//! caller will be responsible for accessing the
	//! synchronized_value after update_values() returns, and
	//! process it.

	void update_values(ONLY IN_THREAD,
			   const std::vector<metrics::axis> &,
			   const std::unordered_map<size_t, int>
			   &requested_col_widths);

	//! Update minimum size.

	bool update_minimum(ONLY IN_THREAD,
			    dim_t minimum);

	//! Recalculate the synchronized axis, with minimums and req widths

	//! Specifies that the sum total of the axis' minimum should be
	//! increase to at least the minimum, and the extra space distributed
	//! according to requested_col_width.
	//!
	//! requested_col_width: key is the column number, weight is the
	//! weighted amount of extra minimums distributed to this axi.

	void update_values(ONLY IN_THREAD,
			   const std::vector<metrics::axis> &,
			   dim_t minimum,
			   const std::unordered_map<size_t, int>
			   &requested_col_widths);
};

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

	friend class synchronized_axis_values_t;

protected:

	//! Scale the derived values

	//! The default implementation finds the synchronized values with
	//! the largest minimum size, then scales the derived values so that
	//! they are at least as the largest minimum.

	virtual std::vector<metrics::axis>
		scale_derived_values(ONLY IN_THREAD,
				     const synchronized_axis_values_t &values,
				     const std::vector<metrics
				     ::derivedaxis> &derived_values);
};

LIBCXXW_NAMESPACE_END

#endif
