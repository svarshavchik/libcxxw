/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef focusable_element_h
#define focusable_element_h

#include "x/w/focusable.H"
#include "focus/focusable_elementfwd.H"

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

//! Implement get_focusable_element() for focusableImplObj.

template<typename element_impl>
child_elementObj &focusable_elementObj<element_impl>::get_focusable_element()
{
	return *this;
}

//! Implement get_focusable_element() for focusableImplObj.

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

LIBCXXW_NAMESPACE_END

#endif
