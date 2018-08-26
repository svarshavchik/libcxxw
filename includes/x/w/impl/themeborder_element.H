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

//! Return the parent element, which hopefully inherits from elementObj::implObj

template<typename element_impl, typename tag>
elementObj::implObj &themeborder1_elementObj<element_impl, tag>
::get_border_element_impl()
{
	return *this;
}

LIBCXXW_NAMESPACE_END

#endif
