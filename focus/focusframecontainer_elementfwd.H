/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef focusframecontainer_elementfwd_h
#define focusframecontainer_elementfwd_h

#include "focus/focusframecontainer_impl.H"

LIBCXXW_NAMESPACE_START

//! A mixin for inheriting from a display element, and a focusframecontainerObj::implObj.

//! The template parameter must be a subclass of containerObj::implObj.
//! This template class multiply-inherits from the
//! template parameter, and focusframecontainerObj::implObj.

template<typename element_impl=containerObj::implObj>
class LIBCXX_HIDDEN focusframecontainer_elementObj
	: public element_impl,
	  public focusframecontainerObj::implObj {

 public:

	using element_impl::element_impl;

	~focusframecontainer_elementObj();

	containerObj::implObj &get_container_impl() override;

	const containerObj::implObj &get_container_impl() const override;

	void keyboard_focus(IN_THREAD_ONLY,
			    focus_change event,
			    const ref<elementObj::implObj> &ptr) override;

};


LIBCXXW_NAMESPACE_END

#endif
