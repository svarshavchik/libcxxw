/*
** Copyright 2018 Double Precision, Inc.
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
	std::vector<metrics::derivedaxis> derived_values;

	//! Recalculate the derived_values.

	//! Invokes all_values' updated(), except the ones whose iterator
	//! gets passed into here. This is the value that was changed, and
	//! the values' element is already responsible for recalculating it,
	//! so no need to do it again.

	void recalculate(ONLY IN_THREAD,
			 std::list<synchronized_axis_value>::iterator
			 iter);
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
			   const std::vector<metrics::axis> &);

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
	//! Compute the derived values.

	//! Invoked from synchronized_axis_values_t::recalculate()
	//!
	//! The default implementation combines all synchronized columns
	//! and produces the derived metric for each synchronized column.
	//! The default implementation does nothing. To create an adjustable
	//! table, the list layout manager overrides this in a subclass to
	//! explicitly set the size of the synchronized axises to their
	//! manually-adjusted values.

	virtual std::vector<metrics::derivedaxis>
		compute_derived_values(ONLY IN_THREAD,
				       const synchronized_axis_values_t &);

};

LIBCXXW_NAMESPACE_END

#endif
