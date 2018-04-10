/*
** Copyright 2017-2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef focusable_element_h
#define focusable_element_h

#include <x/w/focusable.H>
#include <x/w/button_event.H>
#include <x/w/impl/focus/focusable_elementfwd.H>

LIBCXXW_NAMESPACE_START

//! Constructor

//! The constructor forwards its arguments to its superclass, and invokes
//! the superclass's creating_focusable_element().

template<typename element_impl>
template<typename ...Args>
focusable_elementObj<element_impl>::focusable_elementObj(Args && ...args)
	: element_impl(std::forward<Args>(args)...)
{
	element_impl::creating_focusable_element();
}

//! Destructor

template<typename element_impl>
focusable_elementObj<element_impl>::~focusable_elementObj()=default;

//! Implement get_focusable_element() for focusableObj::implObj.

template<typename element_impl>
child_elementObj &focusable_elementObj<element_impl>::get_focusable_element()
{
	return *this;
}

//! Implement get_focusable_element() for focusableObj::implObj.

template<typename element_impl>
const child_elementObj &focusable_elementObj<element_impl>
::get_focusable_element() const
{
	return *this;
}

//! Other focusable elements cannot be created in this container.

//! Blocks creation of child focusable elements.

template<typename element_impl>
void focusable_elementObj<element_impl>::creating_focusable_element()
{
	this->creating_focusable_child_element();
}

//! Button 1 press set_focus_only().

template<typename element_impl>
bool focusable_elementObj<element_impl>
::process_button_event(ONLY IN_THREAD,
		       const button_event &be,
		       xcb_timestamp_t timestamp)
{
	bool flag=false;

	if (be.button == 1 && this->activate_for(be))
	{
		this->set_focus_only(IN_THREAD, &be);
		flag=true;
	}

	if (element_impl::process_button_event(IN_THREAD, be, timestamp))
		flag=true;

	return flag;
}

//! Override set_inherited_visibility().

//! If we are no longer visible, make sure we don't have keyboard focus.

template<typename element_impl>
void focusable_elementObj<element_impl>
::set_inherited_visibility_flag(ONLY IN_THREAD, bool flag)
{
	element_impl::set_inherited_visibility_flag(IN_THREAD, flag);
	if (!flag)
		this->unfocus_later(IN_THREAD);
}

LIBCXXW_NAMESPACE_END

#endif
