/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_container_elementfwd_H
#define x_w_impl_container_elementfwd_H

#include <x/w/impl/container.H>
#include <x/w/impl/element.H>

LIBCXXW_NAMESPACE_START

template<typename element_impl>
class LIBCXX_PUBLIC container_elementObj;

//! A mixin for inheriting from a widget and a containerObj::implObj.

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

	using containerObj::implObj::get_window_handler;

	template<typename ...Args>
	container_elementObj(Args && ...args);

	void do_draw(ONLY IN_THREAD,
		     const draw_info &di,
		     const rectarea &areas) override;

	void inherited_visibility_updated(ONLY IN_THREAD,
					  inherited_visibility_info &info)
		override;

	void process_updated_position(ONLY IN_THREAD,
				      updated_position_info &) override;

	void process_same_position(ONLY IN_THREAD) override;

	void request_visibility_recursive(ONLY IN_THREAD, bool flag) override;

	void draw_after_visibility_updated(ONLY IN_THREAD, bool flag) override;

	void do_for_each_child(ONLY IN_THREAD,
			       const function<void (const element &e)> &)
		override;

	size_t num_children(ONLY IN_THREAD) override;

	void initialize(ONLY IN_THREAD) override;

	void theme_updated(ONLY IN_THREAD, const const_defaulttheme &new_theme)
		override;

	elementObj::implObj &container_element_impl() override;

	const elementObj::implObj &container_element_impl() const override;

	void removed_from_container(ONLY IN_THREAD) override;

	void save(ONLY IN_THREAD, const screen_positions &) override;
};

LIBCXXW_NAMESPACE_END

#endif
