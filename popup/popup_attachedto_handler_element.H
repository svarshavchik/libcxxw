/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef popup_attachedto_handler_element_h
#define popup_attachedto_handler_element_h

#include "popup/popup_attachedto_handler_elementfwd.H"
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

//! Constructor

template<typename element_impl>
template<typename ...Args>
popup_attachedto_handler_elementObj<element_impl>
::popup_attachedto_handler_elementObj(const ref<popup_attachedto_handlerObj>
				      &attachedto_handler, Args && ...args)
	: element_impl(std::forward<Args>(args)...),
	  attachedto_handler(attachedto_handler)
{
}

//! Destructor

template<typename element_impl>
popup_attachedto_handler_elementObj<element_impl>
::~popup_attachedto_handler_elementObj()=default;

//! Override initialize()

//! The attachedto_info needs to be updated.
template<typename element_impl>
void popup_attachedto_handler_elementObj<element_impl>
::initialize(IN_THREAD_ONLY)
{
	element_impl::initialize(IN_THREAD);
	update_attachedto_info(IN_THREAD);
}

//! Override process_updated_position().

//! The attachedto_info needs to be updated.
template<typename element_impl>
void popup_attachedto_handler_elementObj<element_impl>
::process_updated_position(IN_THREAD_ONLY)
{
	element_impl::process_updated_position(IN_THREAD);
	update_attachedto_info(IN_THREAD);
}

//! Override absolute_location_updated().

//! The attachedto_info needs to be updated.

template<typename element_impl>
void popup_attachedto_handler_elementObj<element_impl>
::absolute_location_updated(IN_THREAD_ONLY)
{
	element_impl::absolute_location_updated(IN_THREAD);
	update_attachedto_info(IN_THREAD);
}

//! Update the attachedto_info.

//! So the popup is reposition to reflect our new absolute location.
template<typename element_impl>
void popup_attachedto_handler_elementObj<element_impl>
::update_attachedto_info(IN_THREAD_ONLY)
{
	attachedto_handler->update_attachedto_element_position
		(IN_THREAD,
		 this->get_absolute_location_on_screen(IN_THREAD));
}

LIBCXXW_NAMESPACE_END

#endif
