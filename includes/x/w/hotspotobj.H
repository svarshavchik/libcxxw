/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_hotspotobj_h
#define x_w_hotspotobj_h

#include <x/w/hotspotfwd.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Methods common to all display elements that can be activated.

//! \see hotspot

class hotspotObj : virtual public obj {

public:
	class LIBCXX_HIDDEN implObj;

	//! Internal implementation details.

	const ref<implObj> impl;

	//! Constructor
	hotspotObj(const ref<implObj> &)
		LIBCXX_HIDDEN;

	//! Destructor
	~hotspotObj() LIBCXX_HIDDEN;

	//! Install an activation callback.

	void on_activate(const hotspot_callback_t &);
};

LIBCXXW_NAMESPACE_END

#endif
