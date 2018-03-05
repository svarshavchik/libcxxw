/*
** Copyright 2017-2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef background_color_element_H
#define background_color_element_H

#include "background_color_elementfwd.H"
#include "background_color.H"
#include "element.H"

LIBCXXW_NAMESPACE_START

inline const background_color background_color_element_implObj
::get(IN_THREAD_ONLY)
{
	return color;
}

//! Destructor
template<typename tag>
background_color_element<tag>::~background_color_element()=default;

//! If the element is visible, invoke do_update().

template<typename tag>
void background_color_element<tag>
::update(IN_THREAD_ONLY,
	 const background_color &new_color)
{
	this->do_update(IN_THREAD, new_color,
			this->background_color_element_impl());
}

template<typename tag>
void background_color_element<tag>
::set_background_color_for_element(IN_THREAD_ONLY)
{
	auto &e=this->background_color_element_impl();

	if (!e.data(IN_THREAD).inherited_visibility)
		return;

	background_color_element_implObj
		::set_background_color_for_element(IN_THREAD, e);
}

//! Constructor

//! The first parameter is the initial background_color, the remaining
//! parameters get forwarded to the parent class's constructor.

template<typename element_impl, typename tag>
template<typename ...Args>
inline background_color_1elementObj<element_impl, tag>
::background_color_1elementObj(const background_color &color,
			       Args && ...args)
	: element_impl(std::forward<Args>(args)...),
	  background_color_element<tag>(color)
{
}

//! Constructor

//! The first parameter is the initial background_color, the remaining
//! parameters get forwarded to the parent class's constructor.

template<typename element_impl, typename tag>
template<typename ...Args>
inline background_color_1elementObj<element_impl, tag>
::background_color_1elementObj(const color_arg &color,
			       Args && ...args)
	: element_impl(std::forward<Args>(args)...),
	  background_color_element<tag>(this->create_background_color(color))
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
	background_color_element<tag>::initialize(IN_THREAD);
	background_color_element<tag>::set_background_color_for_element
		(IN_THREAD);

	element_impl::initialize(IN_THREAD);
}

//! New theme

//! Invoke the parent class's theme_updated(), then invoke theme_updated() for
//! the background_color.

template<typename element_impl, typename tag>
void background_color_1elementObj<element_impl, tag>
::theme_updated(IN_THREAD_ONLY, const defaulttheme &new_theme)
{
	background_color_element<tag>::theme_updated(IN_THREAD, new_theme,
						     *this);
	element_impl::theme_updated(IN_THREAD, new_theme);
}


//! Override process_updated_position.

//! If the display element is visible, call background_color_size().

template<typename element_impl, typename tag>
void background_color_1elementObj<element_impl, tag>
::process_updated_position(IN_THREAD_ONLY)
{
	background_color_element<tag>::set_background_color_for_element
		(IN_THREAD);
	element_impl::process_updated_position(IN_THREAD);
}

//! Override invoke_element_state_updates

//! If becoming visible, call set_background_color_for_element().
//! This makes sure we do this work before the display window
//! becomes visible.

template<typename element_impl, typename tag>
void background_color_1elementObj<element_impl, tag>
::invoke_element_state_updates(IN_THREAD_ONLY,
			       element_state::state_update_t s)
{
	if (s == element_state::before_showing)
	{
		background_color_element<tag> &base=*this;

		base.background_color_element_implObj
			::set_background_color_for_element(IN_THREAD, *this);
	}

	element_impl::invoke_element_state_updates(IN_THREAD, s);
}

template<typename element_impl, typename tag>
elementObj::implObj &background_color_1elementObj<element_impl, tag>
::background_color_element_impl()
{
	return *this;
}

LIBCXXW_NAMESPACE_END

#endif
