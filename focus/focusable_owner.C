/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/focusableownerobj.H"
#include "focus/focusable.H"
#include "screen.H"
#include "connection_thread.H"
#include "generic_window_handler.H"
#include "x/w/impl/child_element.H"

LIBCXXW_NAMESPACE_START

focusableObj::ownerObj::ownerObj(const ref<focusableObj::implObj> &impl) : impl(impl)
{
	// The constructor inserts the implementation object into the
	// top level window's focusable_fields.

	impl->get_focusable_element().get_screen()->impl->thread->run_as
		([impl=this->impl]
		 (ONLY IN_THREAD)
		 {
			 impl->focusable_initialize(IN_THREAD);
		 });
}

focusableObj::ownerObj::~ownerObj()
{
	// The destructor is responsible for removing the implementation object
	// from the top level window's focusable_fields.

	// The lambda that gets executed in the connection thread captures
	// impl by reference, so the object is guaranteed to exist.

	impl->get_focusable_element().get_screen()->impl->thread->run_as
		([impl=this->impl]
		 (ONLY IN_THREAD)
		 {
			 impl->focusable_deinitialize(IN_THREAD);
		 });
}

ref<focusableObj::implObj> focusableObj::ownerObj::get_impl() const
{
	return impl;
}

LIBCXXW_NAMESPACE_END
