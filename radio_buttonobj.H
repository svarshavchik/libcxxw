/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef radio_buttonobj_h
#define radio_buttonobj_h

#include "radio_buttonfwd.H"
#include "x/w/elementobj.H"
#include "x/w/callback_triggerfwd.H"
#include "x/w/busyfwd.H"

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

	//! Returns true if this was the previously turned on radio button.

	virtual bool turn_off(ONLY IN_THREAD)=0;

	//! This radio button has been turned off.

	//! If turn_off() returned true, this gets called to provide
	//! any user-specified callbacks.

	virtual void turned_off(ONLY IN_THREAD,
				busy_impl &i_am_busy,
				const callback_trigger_t &trigger)=0;
};

LIBCXXW_NAMESPACE_END

#endif
