/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef focusframecontainer_element_h
#define focusframecontainer_element_h

#include "focus/focusframecontainer.H"
#include "focus/focusframecontainer_elementfwd.H"

LIBCXXW_NAMESPACE_START

template<typename element_impl>
focusframecontainer_elementObj<element_impl>::~focusframecontainer_elementObj()
=default;

template<typename element_impl>
containerObj::implObj &focusframecontainer_elementObj<element_impl>
::get_container_impl()
{
	return *this;
}

template<typename element_impl>
const containerObj::implObj &focusframecontainer_elementObj<element_impl>
::get_container_impl() const
{
	return *this;
}

template<typename element_impl>
void focusframecontainer_elementObj<element_impl>
::keyboard_focus(IN_THREAD_ONLY)
{
	element_impl::keyboard_focus(IN_THREAD);
	focusframecontainerObj::implObj::keyboard_focus(IN_THREAD);
}

template<typename element_impl>
void focusframecontainer_elementObj<element_impl>
::window_focus_change(IN_THREAD_ONLY, bool flag)
{
	element_impl::window_focus_change(IN_THREAD, flag);
	focusframecontainerObj::implObj::window_focus_change(IN_THREAD, flag);
}



LIBCXXW_NAMESPACE_END

#endif
