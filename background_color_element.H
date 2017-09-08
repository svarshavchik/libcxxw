/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef background_color_element_H
#define background_color_element_H

#include "background_color_elementfwd.H"
#include "background_color.H"

LIBCXXW_NAMESPACE_START

inline background_colorptr background_color_element_implObj::get(IN_THREAD_ONLY)
{
	return color;
}

//! Destructor
template<typename tag>
background_color_element<tag>::~background_color_element()=default;

//! Constructor

//! The first parameter is the initial background_color, the remaining
//! parameters get forwarded to the parent class's constructor.

template<typename element_impl, typename tag>
template<typename ...Args>
inline background_color_1elementObj<element_impl, tag>
::background_color_1elementObj(const background_colorptr &color,
			       Args && ...args)
	: background_color_element<tag>(color),
	element_impl(std::forward<Args>(args)...)
{
}

template<typename element_impl, typename tag>
background_color_1elementObj<element_impl, tag>::~background_color_1elementObj()
=default;

//! Display element has been initialized

//! Invoke the parent class's initialize(), then invoke theme_updated() for
//! the background_color, in case the theme has changed in the meantime.
template<typename element_impl, typename tag>
void background_color_1elementObj<element_impl, tag>::initialize(IN_THREAD_ONLY)
{
	element_impl::initialize(IN_THREAD);
	background_color_element<tag>::initialize(IN_THREAD);
}

//! New theme

//! Invoke the parent class's theme_updated(), then invoke theme_updated() for
//! the background_color.

template<typename element_impl, typename tag>
void background_color_1elementObj<element_impl, tag>
::theme_updated(IN_THREAD_ONLY, const defaulttheme &new_theme)
{
	element_impl::theme_updated(IN_THREAD, new_theme);
	background_color_element<tag>::theme_updated(IN_THREAD, new_theme);
}

LIBCXXW_NAMESPACE_END

#endif
