/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef hotspot_bgcolor_h
#define hotspot_bgcolor_h

#include "x/w/hotspot_bgcolor.H"
#include "x/w/hotspotobj.H"
#include "background_colorfwd.H"
#include "connection_thread.H"
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Implementation object for a hotspot with custom background colors

class LIBCXX_HIDDEN hotspot_bgcolorObj::implObj : virtual public obj {

	//! Background color when the hotspot is cold.
	background_color cold_color_thread_only;

	//! Background color when the hotspot is warm.
	background_color warm_color_thread_only;

	//! Background color when the hotspot is hot.
	background_color hot_color_thread_only;
 public:
	THREAD_DATA_ONLY(cold_color);
	THREAD_DATA_ONLY(warm_color);
	THREAD_DATA_ONLY(hot_color);

	//! Constructor
	implObj(const background_color &cold_color,
		const background_color &warm_color,
		const background_color &hot_color);

	//! Destructor
	~implObj();

	//! Get my hotspot implementation object.
	virtual hotspotObj::implObj &get_hotspot_impl()=0;

	//! Get my hotspot implementation object.
	virtual const hotspotObj::implObj &get_hotspot_impl() const=0;

	//! Update the current background color.

	//! The subclass must override this in hotspotObj::implObj,
	//! and invoke this method.
	void temperature_changed(IN_THREAD_ONLY);
};

LIBCXXW_NAMESPACE_END

#endif
