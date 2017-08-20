/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef grabbed_pointerobj_h
#define grabbed_pointerobj_h

#include <x/obj.H>
#include "grabbed_pointerfwd.H"
#include "connection_threadfwd.H"
#include "elementfwd.H"

LIBCXXW_NAMESPACE_START

//! A pointer grab in effect.

//! \see grabbed_pointer.

class LIBCXX_HIDDEN grabbed_pointerObj : virtual public obj {

 public:

	//! Constructor
	grabbed_pointerObj();

	//! Destructor
	~grabbed_pointerObj();

	//! Return the element that owns the grab, if there is one.
	virtual elementimplptr get_grab_element(IN_THREAD_ONLY)=0;

	//! Whether the grab succeeded
	virtual bool succeeded(IN_THREAD_ONLY) const=0;

	//! Allow events
	virtual void allow_events(IN_THREAD_ONLY)=0;

	//! Somebody else wants to grab the pointer too?

	virtual grabbed_pointerptr create_another_grab(IN_THREAD_ONLY,
						       const elementimplptr &)
		=0;
};

LIBCXXW_NAMESPACE_END

#endif
