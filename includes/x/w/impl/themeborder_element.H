/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_themeborder_element_H
#define x_w_impl_themeborder_element_H

#include <x/w/impl/themeborder_elementfwd.H>
#include <x/w/impl/current_border_impl.H>
#include <x/w/impl/border_impl.H>

LIBCXXW_NAMESPACE_START

template<typename tag>
inline themeborder_element_impl<tag>::~themeborder_element_impl()=default;

//! Constructor

//! The first parameter specifies the border, and the remaining
//! parameters get forwarded to the superclass.

template<typename element_impl, typename tag>
template<typename ...Args>
themeborder1_elementObj<element_impl, tag>
::themeborder1_elementObj(const border_arg &arg,
			  Args && ...args)
	: element_impl(std::forward<Args>(args)...),
	  themeborder_element_impl<tag>(arg, *this)
{
}

//! Default destructor
template<typename element_impl, typename tag>
themeborder1_elementObj<element_impl, tag>
::~themeborder1_elementObj()=default;

//! Inherited from elementObj::implObj

//! Invokes the superclass, and the themeborderObj::theme_updated().
//!
//! Addresses the potential race condition with the theme changing before
//! initialization.
template<typename element_impl, typename tag>
void themeborder1_elementObj<element_impl, tag>
::initialize(ONLY IN_THREAD)
{
	element_impl::initialize(IN_THREAD);
	themeborder_element_impl<tag>::initialize(IN_THREAD);
}

//! Inherited from elementObj::implObj

//! Invokes the superclass, and the themeborderObj::theme_updated().

template<typename element_impl, typename tag>
void themeborder1_elementObj<element_impl, tag>
::theme_updated(ONLY IN_THREAD, const defaulttheme &new_theme)
{
	element_impl::theme_updated(IN_THREAD, new_theme);
	themeborder_element_impl<tag>::theme_updated(IN_THREAD, new_theme);
}

//! Return the parent element, which hopefully inherits from elementObj::implObj

template<typename element_impl, typename tag>
elementObj::implObj &themeborder1_elementObj<element_impl, tag>
::get_border_element_impl()
{
	return *this;
}

LIBCXXW_NAMESPACE_END

#endif