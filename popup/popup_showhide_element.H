/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef popup_showhide_element_h
#define popup_showhide_element_h

#include "popup/popup_showhide_elementfwd.H"

LIBCXXW_NAMESPACE_START

//! Constructor

template<typename element_impl>
template<typename ...Args>
popup_showhide_elementObj<element_impl>
::popup_showhide_elementObj(const ref<elementObj::implObj>
			  &popup_element_impl,
			  Args && ...args)
	: element_impl(std::forward<Args>(args)...),
	  popup_element_impl(popup_element_impl)
{
}

//! Destructor

template<typename element_impl>
popup_showhide_elementObj<element_impl>
::~popup_showhide_elementObj()=default;

//! Override activated()

//! Show/hide the other element.

template<typename element_impl>
void popup_showhide_elementObj<element_impl>::activated(IN_THREAD_ONLY)
{
	popup_element_impl
		->request_visibility(IN_THREAD,
				     !popup_element_impl->data(IN_THREAD)
				     .requested_visibility);

	element_impl::activated(IN_THREAD);
}

LIBCXXW_NAMESPACE_END

#endif