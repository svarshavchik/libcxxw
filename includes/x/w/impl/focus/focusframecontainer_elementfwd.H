/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_focusframecontainer_elementfwd_h
#define x_w_impl_focusframecontainer_elementfwd_h

#include <x/w/impl/focus/focusframecontainer_impl.H>
#include <x/w/impl/bordercontainer_elementfwd.H>
#include <x/w/focus_border_appearancefwd.H>
#include <x/w/border_arg.H>

LIBCXXW_NAMESPACE_START

template<typename element_impl>
class LIBCXX_PUBLIC focusframecontainer_elementObj;

//! A mixin for inheriting from a display element, and a focusframecontainer_implObj.

//! The template parameter must be a subclass of containerObj::implObj.
//! This template class multiply-inherits from the template parameter,
//! the themeborder_elementObj template mixin, that handles the focus-off and
//! focus-on borders, and focusframecontainer_implObj.

template<typename element_impl>
class focusframecontainer_elementObj
	: public bordercontainer_elementObj<element_impl>,
	  public focusframecontainer_implObj {

	//! Alias.
	typedef bordercontainer_elementObj<element_impl> superclass_t;

public:

	//! Focus border appearance specification.
	const const_focus_border_appearance focusframe_appearance;

	template<typename ...Args>
	focusframecontainer_elementObj(const const_focus_border_appearance &,
				       const dim_arg &hpad,
				       const dim_arg &vpad,
				       const container_impl &parent_container,
				       Args && ...args);

	~focusframecontainer_elementObj();

	bordercontainer_implObj &focusframe_bordercontainer_impl() override;
	const const_focus_border_appearance &get_appearance() const override;

	void keyboard_focus(ONLY IN_THREAD,
			    const callback_trigger_t &trigger) override;

	void window_focus_change(ONLY IN_THREAD, bool flag) override;
};


LIBCXXW_NAMESPACE_END

#endif
