/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_radio_groupobj_h
#define x_w_radio_groupobj_h

#include <x/obj.H>
#include "radio_groupfwd.H"
#include "x/w/busyfwd.H"
#include "x/w/callback_triggerfwd.H"
#include "x/w/containerobj.H"
#include "radio_buttonfwd.H"
#include "radio_buttonobj.H"
#include <x/weaklistfwd.H>

LIBCXXW_NAMESPACE_START

//! Radio group object.

class radio_groupObj : virtual public obj {

public:
	//! A container for all radio buttons in this group.

	typedef weaklist<radio_buttonObj> button_list_t;

	//! All radio buttons in this group.

	//! All the radio buttons have a strong reference on the radio_group,
	//! and here we store a weak reference back to all of them.

	const button_list_t button_list;

	//! turn_off() everything in button_list except turned_on.

	//! The parent container, busy and trigger get forwarded to
	//! all radio_button_turned_offs.

	void turn_off(ONLY IN_THREAD,
		      const radio_button &turned_on,
		      const container_impl &parent_container,
		      busy_impl &busy,
		      const callback_trigger_t &trigger);
	//! Constructor
	radio_groupObj();

	//! Destructor
	~radio_groupObj();
};

LIBCXXW_NAMESPACE_END

#endif
