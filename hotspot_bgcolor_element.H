/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef hotspot_bgcolor_element_h
#define hotspot_bgcolor_element_h

#include "x/w/hotspot.H"
#include "hotspot_bgcolor_elementfwd.H"
#include "hotspot_element.H"

LIBCXXW_NAMESPACE_START

//! Constructor

template<typename element_impl>
template<typename ...Args>
hotspot_bgcolor_elementObj<element_impl>
::hotspot_bgcolor_elementObj(const background_color &cold_color,
			     const background_color &warm_color,
			     const background_color &hot_color,
			     Args && ...args)
	: hotspot_elementObj<element_impl>(std::forward<Args>(args)...,
					   cold_color),
	  hotspot_bgcolorObj::implObj(cold_color, warm_color, hot_color)
{
}

//! Destructor

template<typename element_impl>
hotspot_bgcolor_elementObj<element_impl>::~hotspot_bgcolor_elementObj()=default;

//! Implement get_hotspot_impl()
template<typename element_impl>
hotspotObj::implObj &hotspot_bgcolor_elementObj<element_impl>
::get_hotspot_impl()
{
	return *this;
}

//! Implement get_hotspot_impl()
template<typename element_impl>
const hotspotObj::implObj &hotspot_bgcolor_elementObj<element_impl>
::get_hotspot_impl() const
{
	return *this;
}


//! Override temperature_changed() from hotspotObj::implObj.

//! And invoke hotspot_bgcolorObj::implObj::temperature_changed(), to
//! update the element's background color.

template<typename element_impl>
void hotspot_bgcolor_elementObj<element_impl>
::temperature_changed(IN_THREAD_ONLY)
{
	hotspot_elementObj<element_impl>::temperature_changed(IN_THREAD);
	hotspot_bgcolorObj::implObj::temperature_changed(IN_THREAD);
}

LIBCXXW_NAMESPACE_END

#endif
