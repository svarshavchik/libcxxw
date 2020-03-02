/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_hotspot_bgcolorobj_h
#define x_w_hotspot_bgcolorobj_h

#include <x/w/hotspot_bgcolorfwd.H>
#include <x/w/hotspotobj.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Methods common to all widgets that change background color when activated.

//! \see hotspot_bgcolor

class hotspot_bgcolorObj : public hotspotObj {

public:
	class LIBCXX_HIDDEN implObj;

	//! Internal implementation details.

	const ref<implObj> impl;

	//! Constructor
	hotspot_bgcolorObj(const ref<implObj> &)
		LIBCXX_HIDDEN;

	//! Destructor
	~hotspot_bgcolorObj() LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
