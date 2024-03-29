/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_container_element_H
#define x_w_impl_container_element_H

#include <x/w/impl/container_elementfwd.H>

LIBCXXW_NAMESPACE_START

//! Constructor

template<typename element_impl>
template<typename ...Args>
container_elementObj<element_impl>::container_elementObj(Args && ...args)
	: element_impl{std::forward<Args>(args)...},
	  containerObj::implObj{static_cast<element_impl &>(*this)}
{
}

//! Override inherited_visibility_updated().

//! Forwarded to containerObj::implObj.

template<typename element_impl>
void container_elementObj<element_impl>
::inherited_visibility_updated(ONLY IN_THREAD,
			       inherited_visibility_info &info)
{
	containerObj::implObj::inherited_visibility_updated_before(IN_THREAD,
								   info);
	element_impl::inherited_visibility_updated(IN_THREAD, info);
	containerObj::implObj::inherited_visibility_updated_after(IN_THREAD,
								  info);
}

//! Override do_draw().

//! Forwarded to containerObj::implObj.

template<typename element_impl>
void container_elementObj<element_impl>::do_draw(ONLY IN_THREAD,
						 const draw_info &di,
						 const rectarea &areas)
{
	containerObj::implObj::do_draw(IN_THREAD, di, areas);
}

template<typename element_impl>
redraw_priority_t container_elementObj<element_impl>
::get_redraw_priority(ONLY IN_THREAD)
{
	return containerObj::implObj::get_redraw_priority(IN_THREAD);
}

//! Override process_updated_position().

//! Forwarded to containerObj::implObj to make any required changes to the
//! child elements' positions. Then forwarded to the parent element's
//! process_updated_position(). The default implementation in
//! in elementObj::implObj calls schedule_full_redraw(), however we also
//! explicitly
//! call schedule_redraw_recursively() since the result of the updated position
//! is also changed position of child elements. Even if the container's
//! size has not changed, and it does not end up changing any child elements'
//! position, the container's position could've been moved, so we better
//! redraw everything.

template<typename element_impl>
void container_elementObj<element_impl>
::process_updated_position(ONLY IN_THREAD)
{
	containerObj::implObj::process_updated_position(IN_THREAD);
	element_impl::process_updated_position(IN_THREAD);
}

//! Override process_same_position().

//! Forwarded to containerObj::implObj to make any required changes to the
//! child elements' positions. Then forwarded to the parent element's
//! process_same_position().

template<typename element_impl>
void container_elementObj<element_impl>
::process_same_position(ONLY IN_THREAD)
{
	containerObj::implObj::process_same_position(IN_THREAD);
	element_impl::process_same_position(IN_THREAD);
}

//! Override request_visibility_recursive().

//! Forwarded to containerObj::implObj and the superclass.
//!
//! If the widget is getting shown, the container gets shown first, then
//! all the elements in the container get shown. If the widget is getting
//! hidden, all elements inthe container get hidden first, then the container.

template<typename element_impl>
void container_elementObj<element_impl>
::request_visibility_recursive(ONLY IN_THREAD, bool flag)
{
	if (!flag)
		element_impl::request_visibility_recursive(IN_THREAD, flag);

	containerObj::implObj::request_child_visibility_recursive
		(IN_THREAD, flag);

	if (flag)
		element_impl::request_visibility_recursive(IN_THREAD, flag);
}

//! Override do_for_each_child().

//! Forwarded to containerObj::implObj.

template<typename element_impl>
void container_elementObj<element_impl>
::do_for_each_child(ONLY IN_THREAD,
		    const function<void (const element &)> &e)
{
	containerObj::implObj::do_for_each_child(IN_THREAD, e);
}

//! Override num_children.

//! Forwarded to containerObj::implObj.

template<typename element_impl>
size_t container_elementObj<element_impl>::num_children(ONLY IN_THREAD)
{
	return containerObj::implObj::num_children(IN_THREAD);
}

//! Override draw_after_visibility_updated().

//! Invokes the superclass's draw_after_visibility_updated(), then invokes
//! containerObj::implObj.

template<typename element_impl>
void container_elementObj<element_impl>
::draw_after_visibility_updated(ONLY IN_THREAD, bool flag)
{
	element_impl::draw_after_visibility_updated(IN_THREAD, flag);
	containerObj::implObj::draw_after_visibility_updated(IN_THREAD, flag);
}

//! Override process_finalized_position().

//! Invokes the superclass's process_finalized_position(), then invokes
//! containerObj::implObj.

template<typename element_impl>
void container_elementObj<element_impl>
::process_finalized_position(ONLY IN_THREAD)
{
	element_impl::process_finalized_position(IN_THREAD);
	containerObj::implObj::process_finalized_position(IN_THREAD);
}

//! Override initialize().

//! Invokes the superclass's initialize(), then invokes
//! containerObj::implObj.

template<typename element_impl>
void container_elementObj<element_impl>::initialize(ONLY IN_THREAD)
{
	element_impl::initialize(IN_THREAD);
	containerObj::implObj::initialize(IN_THREAD);
}

//! Override theme_updated().

//! Invokes the superclass's theme_updated(), then invokes
//! containerObj::implObj.

template<typename element_impl>
void container_elementObj<element_impl>
::theme_updated(ONLY IN_THREAD, const const_defaulttheme &new_theme)
{
	element_impl::theme_updated(IN_THREAD, new_theme);
	containerObj::implObj::theme_updated(IN_THREAD, new_theme);
}

//! Implement container_element_impl().

template<typename element_impl>
elementObj::implObj &container_elementObj<element_impl>::container_element_impl()
{
	return *this;
}

//! Implement container_element_impl().

template<typename element_impl>
const elementObj::implObj &container_elementObj<element_impl>
::container_element_impl() const
{
	return *this;
}

//! Override removed_from_container()

//! Uninstall the layout manager, and recursively invoke all child elements'
//! removed_from_container().

template<typename element_impl>
void container_elementObj<element_impl>
::removed_from_container(ONLY IN_THREAD)
{
	containerObj::implObj::removed_from_container(IN_THREAD);
}

//! Override save()

//! Forward save() to the layout manager
template<typename element_impl>
void container_elementObj<element_impl>::save(ONLY IN_THREAD)
{
	containerObj::implObj::save(IN_THREAD);
	element_impl::save(IN_THREAD);
}

LIBCXXW_NAMESPACE_END

#endif
