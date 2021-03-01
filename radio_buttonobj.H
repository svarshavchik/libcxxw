/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef radio_buttonobj_h
#define radio_buttonobj_h

#include "radio_buttonfwd.H"
#include "x/w/containerobj.H"

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see radio_button

class radio_buttonObj: virtual public obj {

public:
	//! Constructor
	radio_buttonObj() LIBCXX_HIDDEN;

	//! Destructor
	~radio_buttonObj() LIBCXX_HIDDEN;

	//! Turn off this radio button

	virtual void turn_off(ONLY IN_THREAD,
			      const container_impl &parent_container,
			      busy_impl &i_am_busy,
			      const callback_trigger_t &trigger)=0;
};

LIBCXXW_NAMESPACE_END

#endif
