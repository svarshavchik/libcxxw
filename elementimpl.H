/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef elementimpl_h
#define elementimpl_h

#include "x/w/rectangle.H"

#include "connection_threadfwd.H"
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Base class for element implementation.

//! Stuff that all display elements have.

class LIBCXX_HIDDEN elementimplObj : virtual public obj {

	//! Most of the stuff is for the connection thread's consumption, only.

	struct data_thread_only_t {

		//! Nesting level.

		//! Top level window elements have a nesting level of 0.
		//! All others have one more than their parent element.
		size_t nesting_level;

		//! Current position
		rectangle current_position;
	};

	//! For the connection thread's use, only.

	data_thread_only_t data_thread_only;

 public:

	elementimplObj(size_t nesting_level,
		       const rectangle &initial_position);
	~elementimplObj() noexcept;

	THREAD_DATA_ONLY(data);
};

LIBCXXW_NAMESPACE_END

#endif
