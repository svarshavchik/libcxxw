/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_focusframecontainer_element_h
#define x_w_impl_focusframecontainer_element_h

#include <x/w/focus_border_appearance.H>
#include <x/w/impl/focus/focusframecontainer_elementfwd.H>
#include <x/w/impl/bordercontainer_element.H>
#include "x/w/impl/richtext/richtext.H"
#include <x/w/impl/container.H>

LIBCXXW_NAMESPACE_START

//! Constructor

template<typename element_impl>
template<typename ...Args>
focusframecontainer_elementObj<element_impl>::
focusframecontainer_elementObj(const const_focus_border_appearance &appearance,
			       const dim_arg &hpad,
			       const dim_arg &vpad,
			       const container_impl &parent_container,
			       Args && ...args)
	: superclass_t{parent_container->get_window_handler(),
		       appearance->focusoff_border,
		       appearance->focusoff_border,
		       appearance->focusoff_border,
		       appearance->focusoff_border,

		       richtextptr{},
		       0,
		       hpad,
		       vpad,
		       std::forward<Args>(args)...},
	  focusframe_appearance{appearance}
{
}

//! Implement focusframe_bordercontainer_impl().

template<typename element_impl>
bordercontainer_implObj &
focusframecontainer_elementObj<element_impl>::focusframe_bordercontainer_impl()
{
	return *this;
}

//! Destructor
template<typename element_impl>
focusframecontainer_elementObj<element_impl>::~focusframecontainer_elementObj()
=default;

//! Implement get_appearance().

//! Inherited from focusframecontainer_implObj.

template<typename element_impl>
const const_focus_border_appearance &
focusframecontainer_elementObj<element_impl>
::get_appearance() const
{
	return this->focusframe_appearance;
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
