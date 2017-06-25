/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef container_elementfwd_H
#define container_elementfwd_H

#include "container.H"
#include "element.H"

LIBCXXW_NAMESPACE_START

//! A mixin for inheriting from a display element and a containerObj::implObj.

//! The template parameter must be an elementObj::implObj, or one of its
//! subclasses.
//!
//! Multiply inherits from the template parameter, and containerObj::implObj.
//!
//! Overrides elementObj::implObj methods, and takes care of invoking the
//! corresponding methods from containerObj::implObj.
//!
//! The constructor is forwarded to the template parameter.
//! containerObj::implObj does not need construction parameters.

template<typename element_impl>
class container_elementObj : public element_impl,
			     public containerObj::implObj {

public:

	using element_impl::element_impl;

	void do_draw(IN_THREAD_ONLY,
		     const draw_info &di,
		     const rectangle_set &areas) override;

	void inherited_visibility_updated(IN_THREAD_ONLY,
					  inherited_visibility_info &info)
		override;

	void process_updated_position(IN_THREAD_ONLY) override;

	void request_visibility_recursive(IN_THREAD_ONLY, bool flag) override;

	void draw_after_visibility_updated(IN_THREAD_ONLY, bool flag) override;

	void do_for_each_child(IN_THREAD_ONLY,
			       const function<void (const element &e)> &)
		override;

	void theme_updated(IN_THREAD_ONLY, const defaulttheme &new_theme);

	elementObj::implObj &get_element_impl() override;

	const elementObj::implObj &get_element_impl() const override;
};

LIBCXXW_NAMESPACE_END

#endif
