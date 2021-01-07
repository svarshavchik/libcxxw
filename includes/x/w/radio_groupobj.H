/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_radio_groupobj_h
#define x_w_radio_groupobj_h

#include <x/obj.H>
#include <x/w/radio_groupfwd.H>

LIBCXXW_NAMESPACE_START

//! Radio group object.

class radio_groupObj : virtual public obj {

public:
	class LIBCXX_HIDDEN implObj;

	//! Internal implementation details.

	const ref<implObj> impl;

	//! Constructor
	radio_groupObj();

	//! Destructor
	~radio_groupObj() LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
