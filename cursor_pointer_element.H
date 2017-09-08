/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef cursor_pointer_element_h
#define cursor_pointer_element_h

#include "cursor_pointer.H"
#include "connection_threadfwd.H"
#include "cursor_pointer_elementfwd.H"
#include <utility>

LIBCXXW_NAMESPACE_START

//! One of the base classes inherited by an cursor_pointer_element.

//! Constructor
template<typename C>
cursor_pointer_1tag<C>::
cursor_pointer_1tag(const cursor_pointer &c)
	: tagged_cursor_pointer_thread_only(c)
{
}

//! Initialize the cursor_pointer.
template<typename C>
inline void cursor_pointer_1tag<C>::initialize(IN_THREAD_ONLY)
{
	tagged_cursor_pointer(IN_THREAD)=
		tagged_cursor_pointer(IN_THREAD)->initialize(IN_THREAD);
}

//! Reload all cursors after theme_updated().
template<typename C>
inline void cursor_pointer_1tag<C>::theme_updated(IN_THREAD_ONLY,
						  const defaulttheme &new_theme)
{
	tagged_cursor_pointer(IN_THREAD)=
		tagged_cursor_pointer(IN_THREAD)->theme_updated(IN_THREAD,
								new_theme);

}

//! Constructor

template<typename element_impl, typename Tag1, typename ...Tags>
template<typename ...Args>
inline cursor_pointer_element_implObj<element_impl, Tag1, Tags...>
::cursor_pointer_element_implObj(const cursor_pointer &c, Args && ...args)
	: superclass_t(std::forward<Args>(args)...),
	  cursor_pointer_1tag<Tag1>(c)
{
}

//! Override initialize()

//! Also initialize the cursor_pointer in cursor_pointer_1tag.
template<typename element_impl, typename Tag1, typename ...Tags>
inline void cursor_pointer_element_implObj<element_impl, Tag1, Tags...>
::initialize(IN_THREAD_ONLY)
{
	superclass_t::initialize(IN_THREAD);
	cursor_pointer_1tag<Tag1>::initialize(IN_THREAD);
}

//! Override theme_updated()

//! Also update the cursor_pointer_1tag.

template<typename element_impl, typename Tag1, typename ...Tags>
inline void cursor_pointer_element_implObj<element_impl, Tag1, Tags...>
::theme_updated(IN_THREAD_ONLY, const defaulttheme &new_theme)
{
	superclass_t::theme_updated(IN_THREAD, new_theme);
	cursor_pointer_1tag<Tag1>::theme_updated(IN_THREAD, new_theme);
}

LIBCXXW_NAMESPACE_END

#endif
