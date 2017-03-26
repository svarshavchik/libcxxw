/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef hotspot_element_h
#define hotspot_element_h

#include "x/w/hotspot.H"
#include "hotspot_elementfwd.H"
#include "focus/focusable_element.H"

LIBCXXW_NAMESPACE_START

//! Destructor

template<typename element_impl>
hotspot_elementObj<element_impl>::~hotspot_elementObj()=default;

//! Implement get_hotspot_element() for hotspotImplObj.

template<typename element_impl>
child_elementObj &hotspot_elementObj<element_impl>::get_hotspot_element()
{
	return *this;
}

//! Implement get_hotspot_element() for hotspotImplObj.

template<typename element_impl>
const child_elementObj &hotspot_elementObj<element_impl>
::get_hotspot_element() const
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

//! Implement process_key_event(), inherited from elementObj::implObj

//! Handle SPACE and ENTER ourselves, punt for everything else.

template<typename element_impl>
bool hotspot_elementObj<element_impl>
::process_key_event(IN_THREAD_ONLY, char32_t unicode,
		    uint32_t keysym, bool keypress)
{
	return hotspotObj::implObj::process_key_event(IN_THREAD, unicode,
						      keysym, keypress)
		||
		focusable_elementObj<element_impl>
		::process_key_event(IN_THREAD, unicode,
				    keysym, keypress);
}

LIBCXXW_NAMESPACE_END

#endif
