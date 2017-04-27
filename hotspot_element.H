/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef hotspot_element_h
#define hotspot_element_h

#include "x/w/hotspot.H"
#include "x/w/button_event.H"
#include "hotspot_elementfwd.H"
#include "focus/focusable_element.H"

LIBCXXW_NAMESPACE_START

//! Destructor

template<typename element_impl>
hotspot_elementObj<element_impl>::~hotspot_elementObj()=default;

//! Implement get_hotspot_element() for hotspotObj::implObj.

template<typename element_impl>
child_elementObj &hotspot_elementObj<element_impl>::get_hotspot_element()
{
	return *this;
}

//! Implement get_hotspot_element() for hotspotObj::implObj.

template<typename element_impl>
const child_elementObj &hotspot_elementObj<element_impl>::get_hotspot_element()
	const
{
	return *this;
}

//! Implement get_hotspot_focusable() for hotspotObj::implObj.

template<typename element_impl>
focusableImplObj &hotspot_elementObj<element_impl>
::get_hotspot_focusable()
{
	return *this;
}

template<typename element_impl>
const focusableImplObj &hotspot_elementObj<element_impl>
::get_hotspot_focusable() const
{
	return *this;
}

//! Implement keyboard_focus(), inherited from elementObj::implObj

//! Forward it hotspotObj::implObj.

template<typename element_impl>
void hotspot_elementObj<element_impl>::keyboard_focus(IN_THREAD_ONLY,
						      focus_change event,
						      const ref<elementObj::implObj> &ptr)
{
	focusable_elementObj<element_impl>::keyboard_focus(IN_THREAD, event,
							   ptr);
	hotspotObj::implObj::keyboard_focus(IN_THREAD, event, ptr);
}

//! Implement pointer_focus(), inherited from elementObj::implObj

//! Forward it hotspotObj::implObj.

template<typename element_impl>
void hotspot_elementObj<element_impl>::pointer_focus(IN_THREAD_ONLY,
						     focus_change event,
						     const ref<elementObj::implObj> &ptr)
{
	focusable_elementObj<element_impl>::pointer_focus(IN_THREAD, event,
							   ptr);
	hotspotObj::implObj::pointer_focus(IN_THREAD, event, ptr);
}

//! Implement process_key_event(), inherited from elementObj::implObj

//! Handle SPACE and ENTER ourselves, punt for everything else.

template<typename element_impl>
bool hotspot_elementObj<element_impl>
::process_key_event(IN_THREAD_ONLY, const key_event &ke)
{
	return hotspotObj::implObj::process_key_event(IN_THREAD, ke)
		||
		focusable_elementObj<element_impl>
		::process_key_event(IN_THREAD, ke);
}

//! Implement process_button_event(), inherited from elementObj::implObj

//! Button 1 press requests input focus.

template<typename element_impl>
bool hotspot_elementObj<element_impl>
::process_button_event(IN_THREAD_ONLY,
		       const button_event &be,
		       xcb_timestamp_t timestamp)
{
	auto ret=focusable_elementObj<element_impl>
		::process_button_event(IN_THREAD, be, timestamp);

	if (hotspotObj::implObj::process_button_event(IN_THREAD, be,
						      timestamp))
		ret=true;

	return ret;
}

LIBCXXW_NAMESPACE_END

#endif
