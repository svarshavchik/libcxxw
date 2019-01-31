/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef hotspot_bgcolor_h
#define hotspot_bgcolor_h

#include "x/w/hotspot_bgcolor.H"
#include "x/w/hotspotobj.H"
#include "x/w/impl/background_colorfwd.H"
#include "connection_thread.H"
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Implementation object for a hotspot with custom background colors

class LIBCXX_HIDDEN hotspot_bgcolorObj::implObj : virtual public obj {

 public:

	//! Constructor
	implObj();

	//! Destructor
	~implObj();

	//! Get my hotspot implementation object.
	virtual hotspotObj::implObj &get_hotspot_impl()=0;

	//! Get my hotspot implementation object.
	virtual const hotspotObj::implObj &get_hotspot_impl() const=0;

	//! Get my cold color

	virtual background_color cold_color(ONLY IN_THREAD)=0;

	//! Get my warm color
	virtual background_color warm_color(ONLY IN_THREAD)=0;

	//! Get my hot color
	virtual background_color hot_color(ONLY IN_THREAD)=0;

	//! Update the current background color.

	//! The subclass must override this in hotspotObj::implObj,
	//! and invoke this method.
	void temperature_changed(ONLY IN_THREAD,
				 const callback_trigger_t &trigger);
};

LIBCXXW_NAMESPACE_END

#endif
