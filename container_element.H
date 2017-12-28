/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef container_element_H
#define container_element_H

#include "container_elementfwd.H"

LIBCXXW_NAMESPACE_START

//! Override inherited_visibility_updated().

//! Forwarded to containerObj::implObj.

template<typename element_impl>
void container_elementObj<element_impl>
::inherited_visibility_updated(IN_THREAD_ONLY,
			       inherited_visibility_info &info)
{
	containerObj::implObj::inherited_visibility_updated(IN_THREAD, info);
}

//! Override do_draw().

//! Forwarded to containerObj::implObj.

template<typename element_impl>
void container_elementObj<element_impl>::do_draw(IN_THREAD_ONLY,
						 const draw_info &di,
						 const rectangle_set &areas)
{
	containerObj::implObj::do_draw(IN_THREAD, di, areas);
}

//! Override process_updated_position().

//! Forwarded to containerObj::implObj to make any required changes to the
//! child elements' positions. Then forwarded to the parent element's
//! process_updated_position(). The default implementation in
//! in elementObj::implObj calls schedule_redraw(), however we also explicitly
//! call schedule_redraw_recursively() since the result of the updated position
//! is also changed position of child elements. Even if the container's
//! size has not changed, and it does not end up changing any child elements'
//! position, the container's position could've been moved, so we better
//! redraw everything.

template<typename element_impl>
void container_elementObj<element_impl>
::process_updated_position(IN_THREAD_ONLY)
{
	containerObj::implObj::process_updated_position(IN_THREAD);
	element_impl::process_updated_position(IN_THREAD);
	element_impl::schedule_redraw_recursively(IN_THREAD);
}

//! Override request_visibility_recursive().

//! Forwarded to containerObj::implObj.

template<typename element_impl>
void container_elementObj<element_impl>
::request_visibility_recursive(IN_THREAD_ONLY, bool flag)
{
	containerObj::implObj::request_visibility_recursive(IN_THREAD, flag);
}

//! Override do_for_each_child().

//! Forwarded to containerObj::implObj.

template<typename element_impl>
void container_elementObj<element_impl>
::do_for_each_child(IN_THREAD_ONLY,
		    const function<void (const element &)> &e)
{
	containerObj::implObj::do_for_each_child(IN_THREAD, e);
}

//! Override draw_after_visibility_updated().

//! Invokes the superclass's draw_after_visibility_updated(), then invokes
//! containerObj::implObj.

template<typename element_impl>
void container_elementObj<element_impl>
::draw_after_visibility_updated(IN_THREAD_ONLY, bool flag)
{
	element_impl::draw_after_visibility_updated(IN_THREAD, flag);
	containerObj::implObj::draw_after_visibility_updated(IN_THREAD, flag);
}

//! Override initialize().

//! Invokes the superclass's initialize(), then invokes
//! containerObj::implObj.

template<typename element_impl>
void container_elementObj<element_impl>::initialize(IN_THREAD_ONLY)
{
	element_impl::initialize(IN_THREAD);
	containerObj::implObj::initialize(IN_THREAD);
}

//! Override theme_updated().

//! Invokes the superclass's theme_updated(), then invokes
//! containerObj::implObj.

template<typename element_impl>
void container_elementObj<element_impl>
::theme_updated(IN_THREAD_ONLY, const defaulttheme &new_theme)
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

LIBCXXW_NAMESPACE_END

#endif
