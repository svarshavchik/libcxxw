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

//! The first bool parameter constructs focusableImpl, the remaining
//! parameters get forwarded to the element_impl constructor.

template<typename element_impl>
template<typename ...Args>
focusable_elementObj<element_impl>::focusable_elementObj(bool enabled,
							 Args && ...args)
	: element_impl(std::forward<Args>(args)...),
	  focusableImplObj(enabled)
{
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

LIBCXXW_NAMESPACE_END

#endif
