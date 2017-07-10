/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef focusable_elementfwd_h
#define focusable_elementfwd_h

#include "focus/focusable.H"
#include "x/w/button_eventfwd.H"
#include <xcb/xproto.h>

LIBCXXW_NAMESPACE_START

//! A mixin for inherting from a display element, and a focusableImplObj.

//! The template parameter must be a subclass of child_elementObj. This
//! multiply-inherits from the template parameter, and focusableImplObj.

template<typename element_impl=child_elementObj>
class LIBCXX_HIDDEN focusable_elementObj : public element_impl,
					   public focusableImplObj {

 public:

	template<typename ...Args>
		focusable_elementObj(Args && ...args);

	using element_impl::element_impl;

	~focusable_elementObj();

	child_elementObj &get_focusable_element() override;

	const child_elementObj &get_focusable_element() const override;

	void creating_focusable_element() override;

	bool process_button_event(IN_THREAD_ONLY,
				  const button_event &be,
				  xcb_timestamp_t timestamp) override;
};


LIBCXXW_NAMESPACE_END

#endif
