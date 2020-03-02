/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef grabbed_pointerobj_h
#define grabbed_pointerobj_h

#include <x/obj.H>
#include "x/w/impl/grabbed_pointerfwd.H"
#include "x/w/connection_threadfwd.H"
#include "x/w/elementobj.H"

LIBCXXW_NAMESPACE_START

//! A pointer grab in effect.

//! \see grabbed_pointer.

class grabbed_pointerObj : virtual public obj {

 public:

	//! Constructor
	grabbed_pointerObj();

	//! Destructor
	~grabbed_pointerObj();

	//! Return the element that owns the grab, if there is one.
	virtual element_implptr get_grab_element(ONLY IN_THREAD)=0;

	//! Whether the grab succeeded
	virtual bool succeeded(ONLY IN_THREAD) const=0;

	//! Allow events
	virtual void allow_events(ONLY IN_THREAD)=0;

	//! Somebody else wants to grab the pointer too?

	virtual grabbed_pointerptr create_another_grab(ONLY IN_THREAD,
						       const element_implptr &)
		=0;
};

LIBCXXW_NAMESPACE_END

#endif
