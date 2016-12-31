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

	//! Current position
	rectangle current_position_thread_only;

 public:

	elementimplObj(const rectangle &initial_position);
	~elementimplObj() noexcept;

	THREAD_DATA_ONLY(current_position);
};

LIBCXXW_NAMESPACE_END

#endif
