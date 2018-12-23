/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef popup_showhide_element_h
#define popup_showhide_element_h

#include "popup/popup_showhide_elementfwd.H"
#include "popup/popup_attachedto_handler.H"

LIBCXXW_NAMESPACE_START

//! Constructor

template<typename element_impl>
template<typename ...Args>
popup_showhide_elementObj<element_impl>
::popup_showhide_elementObj(const popup_attachedto_handler &my_popup_handler,
			    Args && ...args)
	: element_impl{std::forward<Args>(args)...},
	  my_popup_handler{my_popup_handler}
{
}

//! Destructor

template<typename element_impl>
popup_showhide_elementObj<element_impl>
::~popup_showhide_elementObj()=default;

//! Override activated()

//! Show/hide the other element.

template<typename element_impl>
void popup_showhide_elementObj<element_impl>::activated(ONLY IN_THREAD,
							const callback_trigger_t
							&trigger)
{
	my_popup_handler->toggle_visibility(IN_THREAD);
	element_impl::activated(IN_THREAD, trigger);
}

LIBCXXW_NAMESPACE_END

#endif
