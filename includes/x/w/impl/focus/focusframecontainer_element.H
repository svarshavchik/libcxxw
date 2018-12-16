/*
** Copyright 2017-2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_focusframecontainer_element_h
#define x_w_impl_focusframecontainer_element_h

#include <x/w/impl/focus/focusframecontainer_elementfwd.H>
#include <x/w/impl/bordercontainer_element.H>
#include <x/w/impl/themeborder_element.H>
#include "x/w/impl/richtext/richtext.H"
#include <x/w/impl/container.H>

LIBCXXW_NAMESPACE_START

//! Constructor

template<typename element_impl>
template<typename ...Args>
focusframecontainer_elementObj<element_impl>::
focusframecontainer_elementObj(const border_arg &focusoff_border,
			       const border_arg &focuson_border,
			       const dim_arg &hpad,
			       const dim_arg &vpad,
			       const container_impl &parent_container,
			       Args && ...args)
	: superclass_t{focusoff_border,
		       focuson_border,

		       focusoff_border, focusoff_border, focusoff_border, focusoff_border,
		       richtextptr{},
		       0,
		       hpad,
		       vpad,
		       std::forward<Args>(args)...},
	  focusoff_border{focusoff_border},
	  focuson_border{focuson_border}
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

//! Implement get_focusoff_border().

//! Inherited from focusframecontainer_implObj.

template<typename element_impl>
border_arg focusframecontainer_elementObj<element_impl>
::get_focusoff_border(ONLY IN_THREAD) const
{
	return this->focusoff_border;
}

//! Implement get_focuson_border().

//! Inherited from focusframecontainer_implObj.

template<typename element_impl>
border_arg focusframecontainer_elementObj<element_impl>
::get_focuson_border(ONLY IN_THREAD) const
{
	return this->focuson_border;
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
