/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef radio_group_h
#define radio_group_h

#include "x/w/radio_group.H"
#include "image_button_internalobj.H"

#include <x/weaklist.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN radio_groupObj::implObj : virtual public obj {

public:

	//! A container for all radio buttons in this group.

	typedef weaklist<image_button_internalObj::implObj> button_list_t;

	//! All radio buttons in this group.

	//! All the radio buttons have a strong reference on the radio_group,
	//! and here we store a weak reference back to all of them.

	const button_list_t button_list;

	//! Constructor
	implObj();

	//! Destructor

	~implObj();
};


LIBCXXW_NAMESPACE_END

#endif
