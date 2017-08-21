/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef activated_in_threadobj_h
#define activated_in_threadobj_h

#include "activated_in_threadfwd.H"
#include "connection_threadfwd.H"
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see activated_in_thread

class activated_in_threadObj : virtual public obj {

public:

	//! Constructor
	activated_in_threadObj();

	//! Destructor
	~activated_in_threadObj();

	//! I've been "activated".
	virtual void activated(IN_THREAD_ONLY)=0;

	//! I am enabled.
	virtual bool enabled(IN_THREAD_ONLY)=0;
};

LIBCXXW_NAMESPACE_END

#endif
