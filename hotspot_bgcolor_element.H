/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef hotspot_bgcolor_element_h
#define hotspot_bgcolor_element_h

#include "x/w/hotspot.H"
#include "hotspot_bgcolor_elementfwd.H"
#include "x/w/impl/background_color_element.H"
#include "hotspot_element.H"

LIBCXXW_NAMESPACE_START

//! Destructor

template<typename element_impl>
hotspot_bgcolor_elementObj<element_impl>::~hotspot_bgcolor_elementObj()=default;

//! Implement cold_color().
template<typename element_impl>
std::optional<background_color>
hotspot_bgcolor_elementObj<element_impl>::cold_color(ONLY IN_THREAD)
{
	return background_color_element<hotspot_cold_color>::get(IN_THREAD);
}

//! Initialize the hotspot background color.

template<typename element_impl>
void hotspot_bgcolor_elementObj<element_impl>::initialize(ONLY IN_THREAD)
{
	superclass_t::initialize(IN_THREAD);
	hotspot_bgcolorObj::implObj::temperature_changed(IN_THREAD, {});
}

//! Destructor

template<typename element_impl>
hotspot_transparent_bgcolor_elementObj<element_impl>
::~hotspot_transparent_bgcolor_elementObj()=default;

//! Implement cold_color().
template<typename element_impl>
std::optional<background_color>
hotspot_transparent_bgcolor_elementObj<element_impl>::cold_color(ONLY IN_THREAD)
{
	return std::nullopt;
}


//! Destructor

template<typename superclass_t>
hotspot_bgcolor_elementBaseObj<superclass_t>::~hotspot_bgcolor_elementBaseObj()
=default;

//! Implement get_hotspot_impl()
template<typename superclass_t>
hotspotObj::implObj &hotspot_bgcolor_elementBaseObj<superclass_t>
::get_hotspot_impl()
{
	return *this;
}

//! Implement get_hotspot_impl()
template<typename superclass_t>
const hotspotObj::implObj &hotspot_bgcolor_elementBaseObj<superclass_t>
::get_hotspot_impl() const
{
	return *this;
}

//! Override initialize().

//! Invokes cold_color() to set the initial background color of the hotspot.

template<typename superclass_t>
void hotspot_bgcolor_elementBaseObj<superclass_t>::initialize(ONLY IN_THREAD)
{
	superclass_t::initialize(IN_THREAD);
	hotspot_bgcolorObj::implObj::temperature_changed(IN_THREAD, {});
}

//! Override temperature_changed() from hotspotObj::implObj.

//! And invoke hotspot_bgcolorObj::implObj::temperature_changed(), to
//! update the element's background color.

template<typename superclass_t>
void hotspot_bgcolor_elementBaseObj<superclass_t>
::temperature_changed(ONLY IN_THREAD,
		      const callback_trigger_t &trigger)
{
	superclass_t::temperature_changed(IN_THREAD,
					  trigger);
	hotspot_bgcolorObj::implObj::temperature_changed(IN_THREAD, trigger);
}

//! Implement warm_color().
template<typename superclass_t>
background_color
hotspot_bgcolor_elementBaseObj<superclass_t>::warm_color(ONLY IN_THREAD)
{
	return background_color_element<hotspot_warm_color>::get(IN_THREAD);
}

//! Implement hot_color().
template<typename superclass_t>
background_color
hotspot_bgcolor_elementBaseObj<superclass_t>::hot_color(ONLY IN_THREAD)
{
	return background_color_element<hotspot_hot_color>::get(IN_THREAD);
}

LIBCXXW_NAMESPACE_END

#endif
