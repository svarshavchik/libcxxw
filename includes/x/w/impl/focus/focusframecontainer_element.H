/*
** Copyright 2017-2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_focusframecontainer_element_h
#define x_w_impl_focusframecontainer_element_h

#include <x/w/impl/focus/focusframecontainer_elementfwd.H>
#include <x/w/impl/themeborder_element.H>

LIBCXXW_NAMESPACE_START

//! Destructor
template<typename element_impl>
focusframecontainer_elementObj<element_impl>::~focusframecontainer_elementObj()
=default;

//! Implement get_container_impl().

//! Inherited from focusframecontainer_implObj.

template<typename element_impl>
containerObj::implObj &focusframecontainer_elementObj<element_impl>
::get_container_impl()
{
	return *this;
}

//! Implement get_container_impl().

//! Inherited from focusframecontainer_implObj.

template<typename element_impl>
const containerObj::implObj &focusframecontainer_elementObj<element_impl>
::get_container_impl() const
{
	return *this;
}

//! Implement get_focusoff_border().

//! Inherited from focusframecontainer_implObj.

template<typename element_impl>
current_border_impl focusframecontainer_elementObj<element_impl>
::get_focusoff_border() const
{
	return this->themeborder_element_impl<focusoff_border_tag>
		::current_border;
}


//! Implement get_focuson_border().

//! Inherited from focusframecontainer_implObj.

template<typename element_impl>
current_border_impl focusframecontainer_elementObj<element_impl>
::get_focuson_border() const
{
	return this->themeborder_element_impl<focuson_border_tag>
		::current_border;
}

//! Override keyboard_focus().

//! Forwarded to focusframecontainerObj::implobj.

template<typename element_impl>
void focusframecontainer_elementObj<element_impl>
::keyboard_focus(ONLY IN_THREAD,
		 const callback_trigger_t &trigger)
{
	superclass_t::keyboard_focus(IN_THREAD, trigger);
	focusframecontainer_implObj::keyboard_focus(IN_THREAD, trigger);
}

//! Override window_focus_change().

//! Forwarded to focusframecontainerObj::implobj.

template<typename element_impl>
void focusframecontainer_elementObj<element_impl>
::window_focus_change(ONLY IN_THREAD, bool flag)
{
	superclass_t::window_focus_change(IN_THREAD, flag);
	focusframecontainer_implObj::window_focus_change(IN_THREAD, flag);
}

LIBCXXW_NAMESPACE_END

#endif