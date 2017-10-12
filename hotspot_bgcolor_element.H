/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef hotspot_bgcolor_element_h
#define hotspot_bgcolor_element_h

#include "x/w/hotspot.H"
#include "hotspot_bgcolor_elementfwd.H"
#include "background_color_element.H"
#include "hotspot_element.H"

LIBCXXW_NAMESPACE_START

//! Constructor

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

//! Override initialize().

//! Invokes cold_color() to set the initial background color of the hotspot.

template<typename element_impl>
void hotspot_bgcolor_elementObj<element_impl>::initialize(IN_THREAD_ONLY)
{
	hotspot_elementObj<element_impl>::initialize(IN_THREAD);
	hotspot_bgcolorObj::implObj::temperature_changed(IN_THREAD, {});
}

//! Override temperature_changed() from hotspotObj::implObj.

//! And invoke hotspot_bgcolorObj::implObj::temperature_changed(), to
//! update the element's background color.

template<typename element_impl>
void hotspot_bgcolor_elementObj<element_impl>
::temperature_changed(IN_THREAD_ONLY,
		      const callback_trigger_t &trigger)
{
	hotspot_elementObj<element_impl>::temperature_changed(IN_THREAD, trigger);
	hotspot_bgcolorObj::implObj::temperature_changed(IN_THREAD, trigger);
}

//! Implement cold_color().
template<typename element_impl>
background_color
hotspot_bgcolor_elementObj<element_impl>::cold_color(IN_THREAD_ONLY)
{
	return background_color_element<hotspot_cold_color>::get(IN_THREAD);
}

//! Implement warm_color().
template<typename element_impl>
background_color
hotspot_bgcolor_elementObj<element_impl>::warm_color(IN_THREAD_ONLY)
{
	return background_color_element<hotspot_warm_color>::get(IN_THREAD);
}

//! Implement hot_color().
template<typename element_impl>
background_color
hotspot_bgcolor_elementObj<element_impl>::hot_color(IN_THREAD_ONLY)
{
	return background_color_element<hotspot_hot_color>::get(IN_THREAD);
}

LIBCXXW_NAMESPACE_END

#endif
