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
#include "shortcut/shortcut_activation_element.H"

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
void hotspot_elementObj<element_impl>::keyboard_focus(IN_THREAD_ONLY)
{
	superclass_t::keyboard_focus(IN_THREAD);
	hotspotObj::implObj::keyboard_focus(IN_THREAD);
}

//! Implement pointer_focus(), inherited from elementObj::implObj

//! Forward it hotspotObj::implObj.

template<typename element_impl>
void hotspot_elementObj<element_impl>::pointer_focus(IN_THREAD_ONLY)
{
	superclass_t::pointer_focus(IN_THREAD);
	hotspotObj::implObj::pointer_focus(IN_THREAD);
}

//! Implement process_key_event(), inherited from elementObj::implObj

//! Handle SPACE and ENTER ourselves, punt for everything else.

template<typename element_impl>
bool hotspot_elementObj<element_impl>
::process_key_event(IN_THREAD_ONLY, const key_event &ke)
{
	return hotspotObj::implObj::process_key_event(IN_THREAD, ke)
		||
		superclass_t::process_key_event(IN_THREAD, ke);
}

//! Implement process_button_event(), inherited from elementObj::implObj

//! Button 1 press requests input focus.

template<typename element_impl>
bool hotspot_elementObj<element_impl>
::process_button_event(IN_THREAD_ONLY,
		       const button_event &be,
		       xcb_timestamp_t timestamp)
{
	auto ret=superclass_t::process_button_event(IN_THREAD, be, timestamp);

	if (hotspotObj::implObj::process_button_event(IN_THREAD, be,
						      timestamp))
		ret=true;

	return ret;
}

//! Implement install_shortcut(), inherited from hotspotObj::implObj.

//! Forward it to the shortcut_activation_elementObj mixin.

template<typename element_impl>
void hotspot_elementObj<element_impl>
::install_shortcut(const shortcut &new_shortcut,
		   const activated_in_thread &what_to_activate)
{
	superclass_t::install_shortcut(new_shortcut, what_to_activate);
}

//! Implement uninstall_shortcut(), inherited from hotspotObj::implObj.

//! Forward it to the shortcut_activation_elementObj mixin.

template<typename element_impl>
void hotspot_elementObj<element_impl>::uninstall_shortcut()
{
	superclass_t::uninstall_shortcut();
}

LIBCXXW_NAMESPACE_END

#endif
