/*
** Copyright 2017-2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_focusframecontainer_elementfwd_h
#define x_w_impl_focusframecontainer_elementfwd_h

#include <x/w/impl/focus/focusframecontainer_impl.H>
#include <x/w/impl/bordercontainer_elementfwd.H>
#include <x/w/impl/themeborder_elementfwd.H>
#include <x/w/border_arg.H>

LIBCXXW_NAMESPACE_START

struct focusoff_border_tag;
struct focuson_border_tag;

//! A mixin for inheriting from a display element, and a focusframecontainer_implObj.

//! The template parameter must be a subclass of containerObj::implObj.
//! This template class multiply-inherits from the template parameter,
//! the themeborder_elementObj template mixin, that handles the focus-off and
//! focus-on borders, and focusframecontainer_implObj.

template<typename element_impl>
class focusframecontainer_elementObj
	: public themeborder_elementObj<bordercontainer_elementObj
					<element_impl>,
					focusoff_border_tag,
					focuson_border_tag>,
	  public focusframecontainer_implObj {

	//! Alias.
	typedef themeborder_elementObj<bordercontainer_elementObj<element_impl>,
				       focusoff_border_tag,
				       focuson_border_tag> superclass_t;
 public:

	template<typename ...Args>
	focusframecontainer_elementObj(const border_arg &focusoff_border,
				       const border_arg &focuson_border,
				       const dim_arg &hpad,
				       const dim_arg &vpad,
				       const container_impl &parent_container,
				       Args && ...args);

	~focusframecontainer_elementObj();

	bordercontainer_implObj &focusframe_bordercontainer_impl() override;
	current_border_impl get_focusoff_border() const override;
	current_border_impl get_focuson_border() const override;

	void keyboard_focus(ONLY IN_THREAD,
			    const callback_trigger_t &trigger) override;

	void window_focus_change(ONLY IN_THREAD, bool flag) override;
};


LIBCXXW_NAMESPACE_END

#endif
