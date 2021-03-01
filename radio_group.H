/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef radio_group_h
#define radio_group_h

#include "x/w/radio_group.H"
#include "x/w/busyfwd.H"
#include "x/w/callback_triggerfwd.H"
#include "radio_buttonfwd.H"
#include "radio_buttonobj.H"
#include <x/weaklist.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN radio_groupObj::implObj : virtual public obj {

public:

	//! A container for all radio buttons in this group.

	typedef weaklist<radio_buttonObj> button_list_t;

	//! All radio buttons in this group.

	//! All the radio buttons have a strong reference on the radio_group,
	//! and here we store a weak reference back to all of them.

	const button_list_t button_list;

	//! Constructor
	implObj();

	//! Destructor
	~implObj();

	void turn_off(ONLY IN_THREAD,
		      const radio_button &turned_on,
		      busy_impl &busy,
		      const callback_trigger_t &trigger);
};


LIBCXXW_NAMESPACE_END

#endif
