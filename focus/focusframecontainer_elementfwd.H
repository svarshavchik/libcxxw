/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef focusframecontainer_elementfwd_h
#define focusframecontainer_elementfwd_h

#include "focus/focusframecontainer_impl.H"
#include "themeborder_elementfwd.H"
#include "x/w/border_arg.H"

LIBCXXW_NAMESPACE_START

struct LIBCXX_HIDDEN focusoff_border_tag;
struct LIBCXX_HIDDEN focuson_border_tag;

//! A mixin for inheriting from a display element, and a focusframecontainerObj::implObj.

//! The template parameter must be a subclass of containerObj::implObj.
//! This template class multiply-inherits from the template parameter,
//! the themeborder_elementObj template mixin, that handles the focus-off and
//! focus-on borders, and focusframecontainerObj::implObj.

template<typename element_impl>
class LIBCXX_HIDDEN focusframecontainer_elementObj
	: public themeborder_elementObj<element_impl,
					focusoff_border_tag,
					focuson_border_tag>,
	  public focusframecontainerObj::implObj {

	//! Alias.
	typedef themeborder_elementObj<element_impl,
				       focusoff_border_tag,
				       focuson_border_tag> superclass_t;
 public:

	using superclass_t::superclass_t;

	~focusframecontainer_elementObj();

	containerObj::implObj &get_container_impl() override;

	const containerObj::implObj &get_container_impl() const override;

	virtual current_border_impl get_focusoff_border() const override;
	virtual current_border_impl get_focuson_border() const override;

	void keyboard_focus(IN_THREAD_ONLY) override;

	void window_focus_change(IN_THREAD_ONLY, bool flag) override;
};


LIBCXXW_NAMESPACE_END

#endif
