/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_focusable_elementfwd_h
#define x_w_impl_focusable_elementfwd_h

#include <x/w/impl/focus/focusable.H>
#include <x/w/button_eventfwd.H>
#include <xcb/xproto.h>

LIBCXXW_NAMESPACE_START

template<typename element_impl=child_elementObj>
class LIBCXX_PUBLIC focusable_elementObj;

//! A mixin for inherting from a widget, and a focusableObj::implObj.

//! The template parameter must be a subclass of child_elementObj. This
//! multiply-inherits from the template parameter, and focusableObj::implObj.

template<typename element_impl>
class focusable_elementObj : public element_impl,
			     public focusableObj::implObj {

 public:

	template<typename ...Args>
		focusable_elementObj(Args && ...args);

	using element_impl::element_impl;

	~focusable_elementObj();

	child_elementObj &get_focusable_element() override;

	const child_elementObj &get_focusable_element() const override;

	void creating_focusable_element() override;

	bool process_button_event(ONLY IN_THREAD,
				  const button_event &be,
				  xcb_timestamp_t timestamp) override;

	bool forward_process_button_event(ONLY IN_THREAD,
					  const button_event &be,
					  xcb_timestamp_t timestamp) override
		LIBCXX_HIDDEN;

	void set_inherited_visibility_flag(ONLY IN_THREAD, bool, bool) override;
};


LIBCXXW_NAMESPACE_END

#endif
