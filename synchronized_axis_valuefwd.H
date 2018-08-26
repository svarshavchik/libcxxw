/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_synchronized_axis_valuefwd_h
#define x_w_synchronized_axis_valuefwd_h

#include "x/w/namespace.H"
#include "x/w/metrics/derivedaxis.H"
#include "x/w/synchronized_axisfwd.H"
#include "x/w/connection_threadfwd.H"
#include <x/mpobj.H>
#include <vector>
#include <list>

LIBCXXW_NAMESPACE_START

class synchronized_axis_valueObj;

/*! One of the values being synchronized by a \ref synchronized_axis "synchronized axis".

\see my_synchronized_axis

*/

typedef ref<synchronized_axis_valueObj> synchronized_axis_value;

//! A constant \ref synchronized_axis_value "axis being synchronized".

//! \see synchronized_axis_value

typedef const_ref<synchronized_axis_valueObj> const_synchronized_axis_value;

//! A nullable pointer reference to a \ref synchronized_axis_value "axis being synchronized".

//! \see synchronized_axis_value

typedef ptr<synchronized_axis_valueObj> synchronized_axis_valueptr;

//! A nullable pointer reference to a const \ref synchronized_axis_value "axis being synchronized".

//! \see synchronized_axis_value

typedef const_ptr<synchronized_axis_valueObj> const_synchronized_axis_valueptr;

//! A structure with derived synchronized axis values, and what goes into them.

struct LIBCXX_HIDDEN synchronized_axis_values_t {

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
			 std::list<synchronized_axis_value>::iterator iter);
};

//! Mutex-protected synchronized_axis_values_t

typedef mpobj<synchronized_axis_values_t> synchronized_values;

//! Keep track of a list element's synchronized values.

//! This is a member of each display element that wishes to "synchronize"
//! its width(s) or height(s) with another element; that is, the same width
//! or height for multiple elements.
//!
//! Normally each display element computes its horizontal or vertical metrics
//! by itself. The same "synchronized_axis" object is referenced by multiple
//! elements that wish to compute a combined, or "derived" horizontal and
//! vertical metrics that accomodate all synchronized elements' metrics, and
//! have all the synchronized elements use the same, derived, metrics.
//!
//! Each display element also has its own "synchronized_value", and the
//! display element initially places its horizontal or vertical metrics there,
//! instead of explicitly using them.
//!
//! Each display element has a my_synchronized_axis member. The constructor
//! receives the shared synchronized_axis object, and its element's
//! individual synchronized_value.
//!
//! my_synchronized_axis inserts my_value into the values list in the
//! axis's implementation object, and my_synchronized_axis is responsible for
//! removing my_value. This is normally done by having the display element
//! implement removed_from_container(), and call the method of the same name
//! here. This is the orderly display element destruction. This allows
//! my_synchronized_axis to remove this element's value from the values list,
//! and recompute the derived metrics without it.
//!
//! If removed_from_contaner() does not happen happen (typically as a result
//! of a thrown exception during construction), the destructor removes
//! the my_value, as a last resort.
//!
//! \par Updating metrics
//!
//! \code
//! std::vector<metrics::axis> v;
//!
//! my_synchronized_axis::lock lock{msa};
//!
//! msa.update_values(v);
//!
//! auto d=lock->derived_values;
//! \endcode
//!
//! The display element computes its new metrics and calls update_values().
//!
//! This computes metrics that represent all display elements' derived
//! metrics (a minimum large enough for al synchronized elements'
//! maximums, a maximum of no more than the smallest maximum, but at least
//! as much as the minimum, and an average of preferred values, bounded by
//! the minimums and the maximums). Then each synchronized elements'
//! updated() callback gets invoked from synchronize(), except that the
//! element that called synchronized() does not have its updated() callback
//! invoked. It's expected that update_values()'s caller is responsible for
//! that.
//!
//! \code
//! class my_derived_valuesObj : public synchronized_axis_valueObj {
//!
//! // ...
//!
//!     void updated(ONLY IN_THREAD, const std::vector<metrics::derivedaxis> &);
//! \endcode
//!
//! The display element implements updated(), and passes an instance of
//! the implementation to the constructor. updated() gets invoked when a
//! synchronized display element calls update_values().
//!
struct LIBCXX_HIDDEN my_synchronized_axis {

 public:
	//! All synchronized axises
	const synchronized_axis axis;

	//! My value being synchronized

	const synchronized_axis_value my_value;

 private:
	//! my_value in axis's all_values list.

	std::list<synchronized_axis_value>::iterator value_list_iterator;
 public:
	//! Constructor
	my_synchronized_axis(const synchronized_axis &axis,
			     const synchronized_axis_value &my_value);

	//! Destructor
	~my_synchronized_axis();

	my_synchronized_axis(const my_synchronized_axis &)=delete;

	my_synchronized_axis &operator=(const my_synchronized_axis &)=delete;

	//! This synchronized value was removed from its container.

	//! It's about to be destroyed. Remove this set of values from
	//! the synchronized list, and take this opportunity to recalculate
	//! the derived values.

	void removed_from_container(ONLY IN_THREAD);

	//! Acquire a lock for the purpose of updating the synchronized values.

	class lock : public synchronized_values::lock {


		my_synchronized_axis &me;

	public:
		//! Constructor
		lock(my_synchronized_axis &);

		//! Destructor
		~lock();

		using synchronized_values::lock::operator->;

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
	};
};

LIBCXXW_NAMESPACE_END

#endif
