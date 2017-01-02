/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef element_h
#define element_h

#include "x/w/rectangle.H"
#include "x/w/screen.H"
#include "connection_threadfwd.H"
#include "elementfwd.H"
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Base class for element implementation.

//! Stuff that all display elements have.

class LIBCXX_HIDDEN elementObj::implObj : virtual public obj {

	//! My screen

	const screen screenref;

	//! Most of the stuff is for the connection thread's consumption, only.

	struct data_thread_only_t {

		//! Nesting level.

		//! Top level window elements have a nesting level of 0.
		//! All others have one more than their parent element.
		size_t nesting_level;

		//! Current position
		rectangle current_position;


		////////////////////////////////////////////////////////////
		//
		// Defaulted values.

		//! Requested visibility.

		//! show()/hide() sets this, then adds this element to the
		//! visibility_updated set.

		bool requested_visibility=false;

		//! Actual visibility.

		//! When the connection thread is done with its
		//! work, it goes through all elements in visibility_updated
		//! and if requested_visibility != actual_visibility,
		//! update_visibility gets called.

		bool actual_visibility=false;
	};

	//! For the connection thread's use, only.

	data_thread_only_t data_thread_only;

 public:

	// We assume that anything that uses the comparison function will
	// enforce thread_only.

	friend class elements_by_nesting_level;

	implObj(const screen &screenref,
		size_t nesting_level,
		const rectangle &initial_position);
	~implObj();

	THREAD_DATA_ONLY(data);
 public:
	inline screen get_screen() { return screenref; }

	//! The window's screen.
	inline const_screen get_screen() const { return screenref; }

	/////////////////////////////////////////////////////////////////////


	//! show() or hide() this display element.

	void request_visibility(bool flag);

	//! Executed in the connection thread, to update the actual visibility.

	void update_visibility(IN_THREAD_ONLY);

	//! update_visibility() invokes this callback.

	virtual void visibility_updated(IN_THREAD_ONLY, bool flag);
};

inline bool elements_by_nesting_level::operator()(const elementimpl &a,
						  const elementimpl &b) const
{
	if (a->data_thread_only.nesting_level < b->data_thread_only.nesting_level)
		return true;

	if (b->data_thread_only.nesting_level < a->data_thread_only.nesting_level)
		return false;

	return a < b;
}

LIBCXXW_NAMESPACE_END

#endif
